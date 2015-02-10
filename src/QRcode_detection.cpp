//=================================================================================================
// Copyright (c) 2014, Vibekananda Dutta, WUT
//Faculty of Power and Aeronautical Engineering (MEiL)/ZTMiR Laboratory
// All rights reserved.

//=================================================================================================
#include <list>
#include <cmath>
#include <math.h>/* acos*/
#include <geometry_msgs/Twist.h>
#include "QRcode_detection.h"
#include "QRcode_detection_goal/Goal.h"
#include "QRcode_detection_goal/Pose.h"
#include "QRcode_detection_goal/Markers.h"
#include "QRcode_detection_goal/PerceptInfo.h"
#include <nav_msgs/Odometry.h>
#include <zbar.h>
#include <tf/tf.h> // Geometry transformations
#include <geometry_msgs/Pose2D.h> // Velocity messages
#include "CSVReader.h"
#include <iomanip>
#include <sensor_msgs/LaserScan.h> // Laser sensor messages
#include <sensor_msgs/PointCloud2.h> // PointCloud sensor messages
#define threshold 0.5

using namespace zbar;
using namespace std;

using QRcode_detection_goal::Pose;
using QRcode_detection_goal::Goal;
using Robot::length;
using Robot::bound;
using Robot::rCoordTransform;
using geometry_msgs::Twist;
using geometry_msgs::Point;
using geometry_msgs::Pose2D; 
using sensor_msgs::LaserScan;

QRcode_detection_goal::Pose odomPose;
Robot::SimplePose prePose;
float true_lin_vel, true_ang_vel;
bool odom_updated = false;
ros::Publisher pose_pub;

// Control iterations between map file updates
int iteration = 0;
// Save robot poses to a file
std::ofstream outfile;

double closest_front_obstacle, closest_left_obstacle, closest_right_obstacle;
bool laser_updated = false;
Twist vel;

QRcodeDetection::QRcodeDetection()
{
            
    ros::NodeHandle n;
    ros::NodeHandle p_n("~"); //private nh
   
    scanner_ = new zbar::ImageScanner;
    scanner_->set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
    rotation_image_size_ = 2;
    seenQRPose.x = -1.0; seenQRPose.y = -1.0;
    qrCount =0;
    goal.x = 0; goal.y = 0;
    localization_from_markers_updated = false, laser_update = false;
    image_transport::ImageTransport it(n);
    image_transport::ImageTransport rotated_image_it(p_n);
    image_transport::ImageTransport qrcode_image_it(p_n);
    image_sub_ = it.subscribe("/camera/rgb/image_raw", 1, &QRcodeDetection::imageCallback, this);
    percept_publisher_ = n.advertise<QRcode_detection_goal::Pose>("image_percept", 100);
    rotated_image_pub_ = rotated_image_it.advertiseCamera("image_rotated", 10);
    qrcode_image_pub_ = qrcode_image_it.advertiseCamera("image_qrcode", 100);
    qrCodeCountPublisher = n.advertise<Goal>("ch_qrcodecount", 10);
    obstacle_pub = n.advertise<Goal>("obstacle_publish", 10);
    obsPublisher = n.advertise<Goal>("obstcale_update", 1);
    netForcePublisher = n.advertise<Goal>("Netforce_update", 1);
    cmdVelPub = n.advertise<geometry_msgs::Twist>("/RosAria/cmd_vel", 100);
    cv::namedWindow("view");
    cv::namedWindow("Qrcode");
    cv::namedWindow("Rotation Image");
 
    
}
 

QRcodeDetection::~QRcodeDetection() {
   cv::destroyWindow("view");
   cv::destroyWindow("Qrcode");
   cv::destroyWindow("Rotataion Image");
}

/**
* The APF avoider to use.
*/

void QRcodeDetection::imageCallback(const sensor_msgs::ImageConstPtr& img)
{
  //cv_bridge::CvImageConstPtr cv_image(cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::MONO8));

  cv_bridge::CvImageConstPtr cv_image;
  cv_image = cv_bridge::toCvShare(img, "mono8");
  cv::Mat rotation_matrix = cv::Mat::eye(2,3,CV_32FC1);
  double rotation_angle = 0.0;
    
  ROS_INFO("Received new image with %u x %u pixels.", img->width, img->height);
  cv::imshow("Rotation Image", rotation_matrix);
   cv::waitKey(3);
   
    // calculate rotation angle
    tf::StampedTransform transform;
    tfScalar roll, pitch, yaw;
    transform.getBasis().getRPY(roll, pitch, yaw);
    rotation_angle = -roll;
    ROS_INFO("Rotation_angle: %lf",  rotation_angle);

    // Transform the image.
      cv::Mat in_image = cv_image->image;

      // Compute the output image size.
      int max_dim = in_image.cols > in_image.rows ? in_image.cols : in_image.rows;
      int min_dim = in_image.cols < in_image.rows ? in_image.cols : in_image.rows;
      int noblack_dim = min_dim / sqrt(2);
      int diag_dim = sqrt(in_image.cols*in_image.cols + in_image.rows*in_image.rows);
      int out_size;
      int candidates[] = { noblack_dim, min_dim, max_dim, diag_dim, diag_dim }; // diag_dim repeated to simplify limit case.
      int step = rotation_image_size_;
      out_size = candidates[step] + (candidates[step + 1] - candidates[step]) * (rotation_image_size_ - step);
      ROS_INFO("out_size: %d", out_size);

      // Compute the rotation matrix.
      rotation_matrix = cv::getRotationMatrix2D(cv::Point2f(in_image.cols / 2.0, in_image.rows / 2.0), 180 * rotation_angle / PI, 1);
      rotation_matrix.at<double>(0, 2) += (out_size - in_image.cols) / 2.0;
      rotation_matrix.at<double>(1, 2) += (out_size - in_image.rows) / 2.0;

                 vel.linear.x = 0.05;
                 vel.angular.z = 0.0;
      
      // Do the rotation
      cv_bridge::CvImage *temp = new cv_bridge::CvImage(*cv_image);
      cv::warpAffine(in_image, temp->image, rotation_matrix, cv::Size(out_size, out_size));
      //cv_image.reset(temp);

      sensor_msgs::CameraInfo::Ptr info;
      info.reset(new sensor_msgs::CameraInfo());
      info->header = img->header;

      if (rotated_image_pub_.getNumSubscribers() > 0) {
                
       cv_bridge::CvImage cvImg;
      //img_motion.copyTo(cvImg.image);
      cvImg.header = img->header;
      cvImg.image = temp->image;
      cvImg.encoding = sensor_msgs::image_encodings::MONO8;
      rotated_image_pub_.publish(cvImg.toImageMsg(), info);
        
      }
   
     cv::imshow("view", temp->image);
       cv::waitKey(3);
  // wrap image data
  Image zbar(cv_image->image.cols, cv_image->image.rows, "Y800", cv_image->image.data, cv_image->image.cols * cv_image->image.rows);

  // scan the image for barcodes
  scanner_->scan(zbar);

  // extract results
   percept.header = img->header;
  //percept.camera_info = *camera_info;
  percept.info.class_id = "qrcode";
  percept.info.class_support = 1.0;

   int counter =0;


  for(Image::SymbolIterator symbol = zbar.symbol_begin(); symbol != zbar.symbol_end(); ++symbol) {

    // focal length(calculated before) and test distance
       float f = 525.0, D = 45.0;   

   std::cout<< "Decoded " << symbol->get_type_name() << " symbol \"" << symbol->get_data() << '"' << std::endl;
   std::ofstream outputFile;
   outputFile.open("Decoded_QRcode.txt",std::ios_base::trunc);
   outputFile << "Decoded " << symbol->get_type_name() << " symbol \"" << symbol->get_data() << '"' << std::endl;
   outputFile.close();

    percept.info.object_id = ros::this_node::getName() + "/" + symbol->get_data();
    percept.info.object_id = symbol->get_data();
    percept.info.object_support = 1.0;
    percept.info.name = symbol->get_data();
    percept.info.orientation = orientation;
    if (symbol->get_location_size() != 4) {
      ROS_WARN("Could not get symbol locations(location_size != 4)");
      continue;
    }

    // point order is left/top, left/bottom, right/bottom, right/top
     int min_x = 99999999, min_y = 99999999, max_x = 0, max_y = 0;
    for(int i = 0; i < 4; ++i) {
      if (symbol->get_location_x(i) > max_x) max_x = symbol->get_location_x(i);//p[0]
      if (symbol->get_location_x(i) < min_x) min_x = symbol->get_location_x(i);//p[1]
      if (symbol->get_location_y(i) > max_y) max_y = symbol->get_location_y(i);//p[2]
      if (symbol->get_location_y(i) < min_y) min_y = symbol->get_location_y(i);//p[3]
         point[i].x = symbol->get_location_x(i);
         point[i].y = symbol->get_location_y(i); 
        
        }
          counter++;
          std::istringstream(symbol->get_data()) >> barcodeX;
          std::istringstream(symbol->get_data()) >> barcodeY; 
          //orientation = symbol->get_data();       
     // Length of pixels top left and bottom left
       lengthPixelL = abs(point[1].y - point[0].y);

     // Length of pixels top right and bottom right
        lengthPixelR = abs(point[3].y - point[2].y);
  
      ROS_INFO_STREAM("The length of pixels are "<<lengthPixelL << " " << lengthPixelR);

     // Calculate the distance from the qrcode to camera
        distanceL = (f * D) / lengthPixelL;
        distanceR = (f * D) / lengthPixelR;
        squareDistanceL = pow(distanceL, 2);
        squareDistanceR = pow(distanceR, 2);
        ROS_INFO("Distance from qr code :Left_side: %f Right_side: %f", squareDistanceL, squareDistanceR);

           // Calculate Average and convert to meters
       distanceAvg = ((distanceL + distanceR) / 2)/299.8701;
       ROS_INFO("Average distance in meter: %fm", distanceAvg);

    // rotate the percept back
    cv::Vec3f left_top_corner(min_x, min_y, 1.0f);
    cv::Vec3f right_bottom_corner(max_x, max_y, 1.0f);

        
    // TODO: calculate the inverse transformation of rotation_matrix
   
     qrcodeX = (left_top_corner(0) + right_bottom_corner(0)) / 2;
     qrcodeY = (left_top_corner(1) + right_bottom_corner(1)) / 2;
      barcodeXavg = (point[0].x + point[1].x + point[2].x + point[3].x) / 4;
       
     //ROS_INFO("location: min_x: %d  min_y: %d  max_x: %d  max_y: %d", min_x, min_y, max_x, max_y);

     offsetDistance = -(5 * (960 - (barcodeXavg)) / lengthPixelL) / 299.8701;
         ROS_INFO_STREAM("offset!"<<offsetDistance);
     //Calculating the angle from the barcode to camera
          float tmp1 = min(1.0f, ((squareDistanceR + 25 - squareDistanceL) / (2 * distanceR * 5)));
          angleR = cos(tmp1);
          float tmp2 = min(1.0f, ((squareDistanceL + 25 - squareDistanceR) / (2 * distanceL * 5)));
          angleL = (PI) - (cos(tmp2));
          angleAvg = (angleR + angleL) / 2;
          ROS_INFO_STREAM("ANGLE R:"<< angleR <<":" <<"ANGLE L:"<< angleL<<":" << "AVERAGE :"<< angleAvg);
         angle1 = (angleAvg) * 180.0 / PI;
        ROS_INFO_STREAM("The angle from the qrcode to camera1!"<<angle1<<"degree");
      cbx = offsetDistance;
      cby = sqrt((distanceAvg * distanceAvg) - (offsetDistance * offsetDistance));
      cbtheta = angleAvg;
   
      alpha = cos (offsetDistance / distanceAvg);
      ROS_INFO_STREAM("ALPHA!"<<alpha);
      gamma = ((PI) - (alpha + angleAvg));
      ROS_INFO_STREAM("GAMMA!"<<gamma);
      angle = (alpha) * 180.0 / PI;
      ROS_INFO_STREAM("The angle from the qrcode to camera!"<<angle<<"degree");
      bcx = distanceAvg * cos((PI / 2) - gamma);
      bcy = distanceAvg * sin((PI / 2) - gamma);
      bctheta = PI / 2 + cbtheta;
      ROS_INFO_STREAM("BCTHETA:!"<<bctheta);
       
            float camx, camy = 0.0;
             ROS_INFO_STREAM("QRcode: " << symbol->get_data());
             if (orientation.compare("N") == 0) {
                camx = barcodeX + bcx;
                camy = barcodeY + bcy;
              } else if (orientation.compare("S") == 0) {
                 camx = barcodeX - bcx;
                 camy = barcodeY - bcy;
                 bctheta += PI;
                       } else if (orientation.compare("E") == 0) {
                        camx = barcodeX + bcy;
                        camy = barcodeY - bcx;
                        bctheta += PI * 1.5;
                            } else if (orientation.compare("W") == 0) {
                                     camx = barcodeX - bcy;
                                     camy = barcodeY + bcx;
                                     bctheta += PI * 0.5;
                                 }
                   ROS_INFO_STREAM("camx:, camy:, bctheta:"<< camx << camy << bctheta);    

               float robx, roby;
                               if(orientation.compare("E") == 0){
                                bctheta+=PI;
                                robx = camx + CAMERA_OFFSET*cos(bctheta+PI/2);
                                roby = camy + CAMERA_OFFSET*sin(bctheta+PI/2);
                              }
                               else{
                                     robx = camx + CAMERA_OFFSET*cos(bctheta+PI/2);
                                     roby = camy + CAMERA_OFFSET*sin(bctheta+PI/2);
                               } 
                                
            percept.x      = robx;//qrcodeX;
            percept.y      = roby;//qrcodeY;
            percept.theta  = bctheta;
            percept.width  = right_bottom_corner(0) - left_top_corner(0);
            percept.height = right_bottom_corner(1) - left_top_corner(1);
            percept.distance = distanceAvg;

            percept_publisher_.publish(percept);

            QRcodeDetection::checkIfNewQR(percept);

            //ROS_INFO_STREAM(percept);

            QRcodeDetection::APF(percept);

          ROS_INFO("percept:  x: %.4f  y: %.4f  width: %f  height: %f", percept.x, percept.y, percept.width, percept.height);
          cv::Rect rect(cv::Point2i(std::max(min_x, 0), std::max(min_y, 0)), cv::Point2i(std::min(max_x, cv_image->image.cols), std::min(max_y, cv_image->image.rows)));
          cv_bridge::CvImagePtr qrcode_cv(new cv_bridge::CvImage(*cv_image));
          qrcode_cv->image = cv_image->image(rect); 
      
        // We calculate the angle (180 degree) as half cicle, because the camera angular view is half of the circle, if more than 90 degree right else left  
           if(angle1 >=90.50 && angle1 <=125.50){
                        ROS_INFO("The QR code seems to be in right hand side, let's move to face the QR");
                        vel.linear.x = 0.05;
                        vel.angular.z = 0.05;                          
             }
             else if(angle1 < 90.00 && angle1 >= 40.00){
                    ROS_INFO("The QR code seems to be in left hand side, let's move to face the QR");
                    vel.linear.x = 0.05;
                    vel.angular.z = -0.05; 
               }
               else if(angle1>=90.00 && angle1 < 90.50){
                 ROS_INFO("The QR code seems to be in front, let's move forward");
                 vel.linear.x = 0.05;
                 vel.angular.z = 0.0;
             } 
                else {
                     vel.linear.x = 0.05;
                     vel.angular.z = 0.05;
                     ROS_INFO("I am lost in terms of angular view, Please help me out"); 
             } 
      // As we know the position of the QR code in enviroment, so we use switch statement and provide property to proced the QR code
   switch (1){

          case 1: if(symbol->get_data() == "Wall"){ 
     
                  ROS_INFO("I AM APPROCHING TO THE WALL (QR code:1)");   

                  if(percept.distance <= 0.5){
                      vel.linear.x = 0.0;
                      vel.angular.z = 0.0;
                      ROS_INFO("STOPPED:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);
                      ros::Duration(0.5).sleep();// sleep for one second
                   
                    for (int c =0; c<2; c++){
                     vel.linear.x = -0.03;
                     vel.angular.z = 0.00;
                     ROS_INFO("GOING BACK:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);
                     ros::Duration(0.5).sleep();// sleep for one second                      
                  }
                   for (int c =0; c<3; c++){
                      vel.linear.x = -0.07;
                      vel.angular.z = -0.07;
                      ROS_INFO("TURNING:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);         
                  }
                }
               break;
             } 

   case 2:  if(symbol->get_data() == "Door"){ 
     
            ROS_INFO("I AM APPROCHING TO THE DOOR (QR code:2)");   

            if(percept.distance <= 0.6){
             vel.linear.x = 0.0;
             vel.angular.z = 0.0;
             ROS_INFO("STOPPED: \t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);
             ros::Duration(0.5).sleep();// sleep for one second
             
         for (int c =0; c<3; c++){
             vel.linear.x = -0.03;
             vel.angular.z = 0.00;
             ROS_INFO("GOING BACK:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);
             ros::Duration(0.5).sleep();// sleep for one second                      
           }
               for (int c =0; c<4; c++){
                 vel.linear.x = -0.07;
                 vel.angular.z = -0.07;
                 ROS_INFO("TURNING:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);                             
             }
          }
            break;
     }

   case 3: if(symbol->get_data() == "Alex table"){ 
     
               ROS_INFO("I AM APPROCHING TO THE  ALEX TABLE(QR code:3");   

            if(percept.distance <= 0.6){
             vel.linear.x = 0.0;
             vel.angular.z = 0.0;
             ROS_INFO("STOPPED:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);
             ros::Duration(0.5).sleep();// sleep for one second
             ROS_INFO("I am at goal position");
          }
           break;
     }  

  case 4: if(symbol->get_data() == "Table"){ 
     
            ROS_INFO("I AM APPROCHING TO THE TABLE (QR code:extra)");   

            if(percept.distance <= 0.5){
             vel.linear.x = 0.0;
             vel.angular.z = 0.0;
             ROS_INFO("STOPPED:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);
             ros::Duration(0.5).sleep();// sleep for one second
             
          for (int c =0; c<3; c++){
             vel.linear.x = -0.05;
             vel.linear.y = -0.05;
             vel.angular.z = 0.00;
             ROS_INFO("GOING BACK:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);
                         
           }
              ros::Duration(0.5).sleep();// sleep for one second     
               for (int c =0; c<3; c++){
                 vel.linear.x =  -0.07;
                 vel.angular.z = -0.07;
                 ROS_INFO("TURNING:\t<%.2lf, %.2lf>\n", vel.angular.x, vel.angular.z);                             
              }
          }
         break;
     }

 default :   ROS_INFO("I am lost, Please help me");   
             break;
   }

    if (qrcode_image_pub_.getNumSubscribers() > 0) {
        
      cv_bridge::CvImage cvImg;
      //img_motion.copyTo(cvImg.image);
      cvImg.header = img->header;
      cvImg.image = qrcode_cv->image ;
      cvImg.encoding = sensor_msgs::image_encodings::MONO8;
      qrcode_image_pub_.publish(cvImg.toImageMsg(), info);
            
    }
      cv::imshow("Qrcode", qrcode_cv->image);
      cv::waitKey(3);
             
  }
    cmdVelPub.publish(vel);
 
  // clean up
  zbar.set_data(NULL, 0);
}

bool QRcodeDetection::checkIfNewQR(QRcode_detection_goal::Pose qrPose){
         
             seenQRPose.x = qrPose.x; 
             seenQRPose.y = qrPose.y; 
             seenQRPose.theta = qrPose.theta;
 ROS_INFO("Seen Qrcode position:! x: %.2f, y: %.2f", seenQRPose.x,seenQRPose.y);
             std::stringstream ss; 
             QRcode_detection_goal::Goal topicMsg;
                      ss << "L:\n" << seenQRPose.x <<"\nR:\n"<< seenQRPose.y <<"\ntheta:\n"<< seenQRPose.theta <<"";
                           topicMsg.name = ss.str(); 
                           qrCodeCountPublisher.publish(topicMsg);
                 return true;
}

void getRobotPose(const nav_msgs::Odometry& robot_pose){
/*
 robot_pose.header.frame_id = "odom";
 robot_pose.child_frame_id = "base_link";
 robot_pose.header.stamp = ros::Time::now();
 odomPose.header = robot_pose.header;*/
 
 odomPose.x = robot_pose.pose.pose.position.x;
 odomPose.y = robot_pose.pose.pose.position.y;
 odomPose.theta =  tf::getYaw(robot_pose.pose.pose.orientation);
 ROS_INFO("Robot current position:! x: %.2f, y: %.2f, theta: %.2f---", odomPose.x,odomPose.y,odomPose.theta);
  // Open file to store robot poses
  outfile.open("Postures.txt",std::ios_base::app);
  outfile << "Robot current Pose in X: " << odomPose.x <<"," << "Robot current Pose in Y: " << odomPose.y <<","<<"Robot current Pose in Z: " << odomPose.theta <<'/' << std::endl;
  // Close file
  outfile.close();
 true_lin_vel = robot_pose.twist.twist.linear.x;
 true_ang_vel = robot_pose.twist.twist.angular.z;
 odom_updated = true;
 prePose.x = odomPose.x ;
 prePose.y = odomPose.y;
 prePose.a = odomPose.theta;
 std::stringstream ss; 
         QRcode_detection_goal::Goal topicMsg;
         ss << "Robot current postion:" <<odomPose.x <<"" << odomPose.y<<""<< odomPose.theta <<"" <<  robot_pose.child_frame_id <<""<< robot_pose.header<<"/";
         topicMsg.name = ss.str(); 
         pose_pub.publish(topicMsg);
         
}

list<Polar> QRcodeDetection::scanobjectList(QRcode_detection_goal::Pose& qrPose){
list<Polar> points;
// Create a placeholder with the initial angle.
Polar p, Temp;  
//ROS_INFO_STREAM(qrPose);
float diameter, radius;
float dx = pow((odomPose.x - qrPose.x),2);
float dy = pow((odomPose.y - qrPose.y),2);
p.a = (atan2(dy,dx)) * 180/PI;
Temp.a = p.a;
// Convert scan.ranges into a list.
//diameter = sqrt(pow((odomPose.x - qrPose.x),2) + pow((odomPose.y- qrPose.y),2)) ;
 radius = qrPose.distance;
 for (unsigned int i = 0; i < radius; i++) {
   float d = radius;
   if(d > 0.3 && d <0.9){
       p.d = d;
}
 // New convention: < 0 means invalid.
else{
    p.d = -0.1;
 }
ROS_INFO("Radius:\t(%.2f)", p.d);
points.push_back(p);
p.a += Temp.a;
}
ROS_INFO("Angular Pose:\t(%.2f)", p.a);
return points;

} 

list<Polar> QRcodeDetection::findObjects(list<Polar> points) {
// List of "object" points: local minima of the scan.
list<Polar> objects;
bool conti = false;
// Point of the current object's minimum, for setting and adding to objects.
Polar objMin;
for (list<Polar>::iterator i = points.begin(); i != points.end(); i++) {
ROS_INFO("obj1:\%.2f", *i);
Polar p = *i;
if (p.d >= 0.5 && p.d <=0.9){
// if this point is closer than objMin, save it.
               conti = true;
               objMin = p;
             }
                 
 else {
// not in an object; add the previous object to the list and
// make a new one.
  objects.push_back(objMin);
  objMin = p;
 } 

ROS_INFO("Pose3:\t(%.2f, %.2f)", objMin.d, p.d);
 }
return objects;
}

/**
* Path planning using an artificial potential field technique with cached
* obstacles to assist with the narrow field of view of the Kinect.
* Order of operations:
* 1. Compute goal location relative to robot
* 2. Compute goal force: calcGoalForce
* 3. Turn Kinect scan into list of discrete obstacles: findObjects
* 4. Updates obstacle cache - delete old/far-away obstacles,
* add new obstacles: updateObstacleList
* 5. Add force due to Qrcode position: updateNetForce
* 6. Convert (x,y) force vector to polar (relative to robot heading)
* 7. Convert force vector to command velocity: cmdTransform
*/

Polar QRcodeDetection::APF(QRcode_detection_goal::Pose& qrPose){

   
        std::stringstream ss; 
         QRcode_detection_goal::Goal topicMsg;
         
if(abs(prePose.x - qrPose.x)>1.0 || abs(prePose.y - qrPose.y)>1.0 || abs(prePose.a - qrPose.theta)>1.0 ){// put the odom pose value in prepose and compare with the qrcode pose
    getRobotPose(robot_pose);
    ObstacleList.clear();
    ROS_DEBUG("clearing ObstacleList: %u", ObstacleList.size());
}
     
 //QRcode_detection_goal::Pose odomPose;
  ROS_INFO("Pose:\t(%lf, %lf), <%lf>", prePose.x, prePose.y, prePose.a);
  // The goal is the head of the seen QRCODE.
  goal.x = qrPose.x;
  goal.y = qrPose.y;
 ROS_INFO("Abs Goal:\t(%.4f, %.4f)", goal.x, goal.y);

  // Open file to store goal poses
  outfile.open("Goal.txt",std::ios_base::app);
  outfile << "current goal Pose in X: " << goal.x <<","<< "Current goal Pose in Y: " << goal.y <<'"' << std::endl;
  // Close file
  outfile.close();
 // Convert the goal into the robot reference frame.
Point bPoint;
float dx = goal.x - prePose.x; float dy = goal.y - prePose.y;
float theta = -qrPose.theta;
bPoint.x = dx * cos(theta) - dy * sin(theta);
bPoint.y = dx * sin(theta) + dy * cos(theta);
goalwrtRobot.x = bPoint.x;
goalwrtRobot.y = bPoint.y;
ROS_INFO("Rel Goal:\t(%.2f, %.2f)", goalwrtRobot.x, goalwrtRobot.y);
// The list of "objects" found; already in the robot reference frame.
  list<Polar> objects = QRcodeDetection::findObjects(QRcodeDetection::scanobjectList(qrPose));
       
//calculate goal force
     Point netForce;
     double goalDist = length(goalwrtRobot.x, goalwrtRobot.y);
       if (goalDist <= D_GOAL) {
         netForce.x = (K_GOAL / D_GOAL) * goalwrtRobot.x;
         netForce.y = (K_GOAL / D_GOAL) * goalwrtRobot.y;
           } else {
                 netForce.x = K_GOAL * goalwrtRobot.x / goalDist;
                 netForce.y = K_GOAL * goalwrtRobot.y / goalDist;
              }
           ROS_INFO("GoalF:\t%.2f, %.2f", netForce.x, netForce.y);

   ss << "netForce_X:\n" << netForce.x <<"\n netForce_Y:\n"<< netForce.y << ";";
                           topicMsg.name = ss.str(); 
               obstacle_pub.publish(topicMsg);
               QRcodeDetection::updateObstacleList(objects);
               QRcodeDetection::updateNetForce(netForce);
               //QRcodeDetection::convertRobotToGlobal(p);   
               //QRcodeDetection::pushIfUnique(prePose); 
               QRcodeDetection::distancefromRobot(prePose);
// Resulting command vector. 
   
     p.d = length(netForce.x, netForce.y);
    // ROS_INFO("Polar:\t<%+.4f>\n",  p.d);
    if(abs(netForce.y) <= 0.099 && abs(netForce.x) <= 0.099) {
          ROS_INFO("zero net force detected");
              p.a = 0;
                   }
          else {
              p.a = atan2(netForce.y, netForce.x);
                 }
           ROS_INFO("TotalF:\t<%+.2f, %.2f>", p.d, p.a);

   if(p.a > 2.0){
        
         cmd.a = 0.3;
      //vel.angular.z = 0.1;

     if(p.a < PI/2.0){

        cmd.d =  0.5;
          if(cmd.d >= threshold){
          cmd.d = 0.5;
          }
       //vel.linear.x = - cos(p.a)*p.d;// we need to check with (-)ve value
     }
  else{
        cmd.d = 0.0;
       //vel.linear.x =0.0;
 }
}
  else if(p.a < - 0.12){

         cmd.a = - 0.03;
       //vel.angular.z= - 0.05;

    if(p.a > - PI/2.0){
       cmd.d =  0.5;

       if(cmd.d >= threshold){
        cmd.d = 0.5;
          }
             //vel.linear.x = - cos(p.a)*p.d;// we need to check with (-)ve value
}
  else{
          cmd.d = 0.0;
        //vel.linear.x =0.0;
  } 
 }
  else {
        cmd.a = 0.0;
        cmd.d = p.d;
        //vel.angular.z = 0.0;
        //vel.linear.x = p.d;
}
 if (vel.linear.x >=0.05 && vel.angular.z == 0.0){
      cmd.d = 0.05; 
     
    if(cmd.d >= threshold){
        cmd.d = 0.5;
    }
}
 /*else{
        vel.angular.z = 0.0;
        vel.linear.x = p.d;
}*/
   vel.angular.z = cmd.a;
   vel.linear.x = cmd.d;
   cmdPrev = cmd;
   prePose.x = qrPose.x; prePose.y = qrPose.y; prePose.a = qrPose.theta;
   ROS_INFO("Vel:\t<%+.2f, %.2f>\n",   vel.angular.x, vel.angular.z);
   cmdVelPub.publish(vel);   
   ROS_INFO("NavVel:\t<%+.2f, %.2f>\n",  cmd.d, cmd.a);
   return cmd;
 
}


/*
* Convert the given polar (relative to robot) coordinate to the
* global coordinate system.
*/

bool QRcodeDetection::convertRobotToGlobal(Polar &p){
sp.x = seenQRPose.x + p.d * cos(seenQRPose.theta + p.a);
sp.y = seenQRPose.y + p.d * sin(seenQRPose.theta + p.a);
sp.a = 0;
 ROS_INFO("Simple Pose:\t<%.2lf, %.2lf>\n", sp.x, sp.y);
//return sp;
}

/*
* Add the given point to the obstacle cache if it is farther than
* OBS_MATCH_DIST away from all other cached obstacles.
*/

bool QRcodeDetection::pushIfUnique(Robot::SimplePose &sp){

CachedPoint newObs;// put the value of robot coordinate system in new obs compare to global
newObs.p.x = sp.x;
newObs.p.y = sp.y;
//newObs.lastT = lastScanTime;
ObstacleList.push_back(newObs);
ROS_INFO("Push if unique:\t<%.2f, %.2f>\n", newObs.p.x, newObs.p.y);
//publishing the obstacle's coordinates to the ch_obstacle topic
stringstream ss; 
QRcode_detection_goal::Goal topicMsg;
ss << "(" << newObs.p.x << ", " << newObs.p.y << ")";
topicMsg.name = ss.str(); 
obsPublisher.publish(topicMsg);
return true;
}


double QRcodeDetection::distancefromRobot(Robot::SimplePose &sp){
 
 float distance = sqrt((prePose.x - sp.x)*(prePose.x - sp.x) + (prePose.y - sp.y)*(prePose.y - sp.y));
 ROS_INFO("distance from Robot:%.2f!",distance);
}

/**
* Convert the given point in the global coordinates to the robot's
* relative coordinate system.
*/
void QRcodeDetection::convertFromGlobalToRobotInPolar(Point &sp){

if(abs(sp.y-seenQRPose.y) <= 0.099 && abs(sp.x - seenQRPose.x) <= 0.099){
p.a = 0;
p.d = 0;
ROS_DEBUG("APF, Returning GToR: zeros detected");
}
else{
p.a = atan2(sp.y-seenQRPose.y, sp.x - seenQRPose.x) - seenQRPose.theta;
p.d = sqrt((sp.x-seenQRPose.x)*(sp.x-seenQRPose.x) + (sp.y-seenQRPose.y)*(sp.y-seenQRPose.y));
}

}

/**
* Add obstacle force from all cached obstacles to the given (goal)
* currently active force.
*/
void QRcodeDetection::updateNetForce(Point &netForce){
stringstream ss; QRcode_detection_goal::Goal topicMsg;
//int objIndex = 0;
for (std::vector<CachedPoint>::iterator it = ObstacleList.begin() ; it != ObstacleList.end(); ++it){
Point od = it->p;
ROS_INFO("obj3:\%.2f", &od);
QRcodeDetection::convertFromGlobalToRobotInPolar(od);
double f = K_OBS * (1.0/D_OBS - 1.0/od.x) / (od.x * od.x);
netForce.x += f * cos(od.x);
netForce.y += f * sin(od.y);
ROS_INFO("Obj(%d)F:\t%.2f, %.2f", f * cos(od.x), f * sin(od.x));
}
ss.str(""); ss << "(" << netForce.x << ", " << netForce.y << ")";
topicMsg.name = ss.str(); netForcePublisher.publish(topicMsg);
}

/**
* Update the obstacle cache:
* - Eliminate any obstacles not seen in OBS_CACHE_TIMEOUT seconds
* - Eliminate any obstacles farther than D_OBS away
* - Eliminate any obstacles behind the robot (necessary?)
* - Add any new unique obstacles
* - Update time-last-seen of present obstacles already in cache
*/
void QRcodeDetection::updateObstacleList(list<Polar>& objects){
// now add in new ones if they are close enough to worry about.
for (list<Polar>::iterator it = objects.begin(); it != objects.end(); ++it) {
Polar pointWrtRobot = *it;
ROS_INFO("obj5:\%.2f", *it);
if (pointWrtRobot.d <= D_OBS) {
QRcodeDetection::convertRobotToGlobal(pointWrtRobot);
//Robot::SimplePose ps = QRcodeDetection::convertRobotToGlobal(pointWrtRobot);
ROS_INFO("Polar Object that might be added:\t(%.2f, %.2f)", pointWrtRobot.d, pointWrtRobot.a);
//ROS_DEBUG("Object in global cood:\t(%.2f, %.2f)", pointWrtGlobal->x, pointWrtGlobal->y);
QRcodeDetection::pushIfUnique(sp);
}
}
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "QRcode_detection_goal");
  QRcodeDetection QRD;
  ros::NodeHandle n;
  ros::Subscriber robotpose_sub = n.subscribe("/RosAria/pose", 1, &getRobotPose);
  //ros::Subscriber sub_laser = n.subscribe("/scan", 1, &scanCallback);
  pose_pub = n.advertise<Goal>("pose_publish", 10);
  ros::spin();
 
  return 0;
}
