//=================================================================================================
// Copyright (c) 2014, Vibekananda Dutta, WUT
//Faculty of Power and Aeronautical Engineering (MEiL)/ZTMiR Laboratory
// All rights reserved.

//=================================================================================================
#include <list>
#include <cmath>
#include <geometry_msgs/Twist.h>
#include "QRcode_detection.h"
#include "QRcode_detection/Goal.h"
#include "QRcode_detection/Pose.h"
#include "QRcode_detection/Markers.h"
#include "QRcode_detection/PerceptInfo.h"
#include <nav_msgs/Odometry.h>
#include <zbar.h>
#include <tf/tf.h> // Geometry transformations
#include <geometry_msgs/Pose2D.h> // Velocity messages
#include "CSVReader.h"
#include <iomanip>
#include <sensor_msgs/LaserScan.h> // Laser sensor messages
#include <sensor_msgs/PointCloud2.h> // PointCloud sensor messages

using namespace zbar;
using namespace std;

using QRcode_detection::Pose;
using QRcode_detection::Goal;
using Robot::length;
using Robot::bound;
using Robot::rCoordTransform;
using geometry_msgs::Twist;
using geometry_msgs::Point;
using geometry_msgs::Pose2D; 

QRcode_detection::Pose odomPose;
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
    localization_from_markers_updated = false, laser_update = false;
    image_transport::ImageTransport it(n);
    image_transport::ImageTransport rotated_image_it(p_n);
    image_transport::ImageTransport qrcode_image_it(p_n);
    image_sub_ = it.subscribe("/camera/rgb/image_raw", 1, &QRcodeDetection::imageCallback, this);
    percept_publisher_ = n.advertise<QRcode_detection::Pose>("image_percept", 100);
    rotated_image_pub_ = rotated_image_it.advertiseCamera("image_rotated", 10);
    qrcode_image_pub_ = qrcode_image_it.advertiseCamera("image_qrcode", 100);
    qrCodeCountPublisher = n.advertise<Goal>("ch_qrcodecount", 10);
    obstacle_pub = n.advertise<Goal>("obstacle_publish", 10);
    obsPublisher = n.advertise<Goal>("obstcale_update", 1);
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
//QRcodeDetection* apf;

void QRcodeDetection::imageCallback(const sensor_msgs::ImageConstPtr& img)//, const sensor_msgs::CameraInfoConstPtr& camera_info)
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

 // int counter =0;


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
    //int min_x = 99999999, min_y = 99999999, max_x = 0, max_y = 0;
    for(int i = 0; i < 4; ++i) {
     /* if (symbol->get_location_x(i) > max_x) max_x = symbol->get_location_x(i);//p[0]
      if (symbol->get_location_x(i) < min_x) min_x = symbol->get_location_x(i);//p[1]
      if (symbol->get_location_y(i) > max_y) max_y = symbol->get_location_y(i);//p[2]
      if (symbol->get_location_y(i) < min_y) min_y = symbol->get_location_y(i);//p[3]*/
         point[i].x = symbol->get_location_x(i);
         point[i].y = symbol->get_location_y(i); 
        
        }
          std::istringstream(symbol->get_data()) >> barcodeX;
          std::istringstream(symbol->get_data()) >> barcodeY;         
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
    cv::Vec3f left_top_corner(point[1].y, point[0].y, 1.0f);
    cv::Vec3f right_bottom_corner(point[3].y, point[2].y, 1.0f);

        
    // TODO: calculate the inverse transformation of rotation_matrix
   
     qrcodeX = (left_top_corner(0) + right_bottom_corner(0)) / 2;
     qrcodeY = (left_top_corner(1) + right_bottom_corner(1)) / 2;
      barcodeXavg = (point[0].x + point[1].x + point[2].x + point[3].x) / 4;
       
     //ROS_INFO("location: min_x: %d  min_y: %d  max_x: %d  max_y: %d", min_x, min_y, max_x, max_y);

     offsetDistance = -(5 * (800 - (barcodeXavg)) / lengthPixelL) / 299.8701;
    
     //Calculating the angle from the barcode to camera
          float tmp1 = min(1.0f, ((squareDistanceR + 25 - squareDistanceL) / (2 * distanceR * 5)));
          angleR = acos(tmp1);
          float tmp2 = min(1.0f, ((squareDistanceL + 25 - squareDistanceR) / (2 * distanceL * 5)));
          angleL = (PI) - (acos(tmp2));
          angleAvg = (angleR + angleL) / 2;
          ROS_INFO_STREAM("ANGLE R:"<< angleR <<":" <<"ANGLE L:"<< angleL<<":" << "AVERAGE :"<< angleAvg);

      cbx = offsetDistance;
      cby = sqrt((distanceAvg * distanceAvg) - (offsetDistance * offsetDistance));
      cbtheta = angleAvg;
   
      alpha = acos(offsetDistance / distanceAvg);
      ROS_INFO_STREAM("ALPHA!-----------"<<alpha);
      gamma = ((PI) - (alpha + angleAvg));
      angle = (gamma * 180) / PI;
      ROS_INFO_STREAM("The angle from the qrcode to camera!----------"<<angle<<"degree");
      bcx = distanceAvg * cos((PI / 2) - gamma);
      bcy = distanceAvg * sin((PI / 2) - gamma);
      bctheta = PI / 2 + cbtheta;
      ROS_INFO_STREAM("BCTHETA:! ----------"<<bctheta);
       
            float camx, camy = 0.0;
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
           
            if(percept.distance <0.5){
             vel.linear.x = 0.0;
             vel.angular.z = 0.0;
            }
       
          else{
                 vel.linear.x = 0.05;
                 vel.angular.z = 0.0;
        }      
               
           
          ROS_INFO("percept:  x: %.4f  y: %.4f  width: %f  height: %f", percept.x, percept.y, percept.width, percept.height);
          cv::Rect rect(cv::Point2i(std::max(point[1].y, 0), std::max(point[0].y, 0)), cv::Point2i(std::min(point[3].y, cv_image->image.cols), std::min(point[2].y, cv_image->image.rows)));
          cv_bridge::CvImagePtr qrcode_cv(new cv_bridge::CvImage(*cv_image));
          qrcode_cv->image = cv_image->image(rect);
         

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

bool QRcodeDetection::checkIfNewQR(QRcode_detection::Pose qrPose){
         
             seenQRPose.x = qrPose.x; 
             seenQRPose.y = qrPose.y; 
             seenQRPose.theta = qrPose.theta;
             std::stringstream ss; 
             QRcode_detection::Goal topicMsg;
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
 ROS_INFO("Robot current position:! x: %.4f, y: %.4f, theta: %.4f---", odomPose.x,odomPose.y,odomPose.theta);
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
         QRcode_detection::Goal topicMsg;
         ss << "Robot current postion:" <<odomPose.x <<"" << odomPose.y<<""<< odomPose.theta <<"" <<  robot_pose.child_frame_id <<""<< robot_pose.header<<"/";
         topicMsg.name = ss.str(); 
         pose_pub.publish(topicMsg);
         
}

list<Polar> QRcodeDetection::scanobjectList(QRcode_detection::Pose& odomPose){
list<Polar> points;
// Create a placeholder with the initial angle.
Polar p;  QRcode_detection::Pose qrPose;
float diameter, radius;
float dx = (odomPose.x - qrPose.x);
float dy = (odomPose.y - qrPose.y);
p.a = atan2(dy,dx) * 180/PI;
// Convert scan.ranges into a list.
diameter = sqrt(pow((odomPose.x - qrPose.x),2) + pow((odomPose.y- qrPose.y),2)) ;
//radius = diameter / 2;
ROS_INFO("Pose:\t(%f, %f)", radius, diameter);
  if(diameter> 0.5 && diameter <0.9){
   p.d = diameter;
}
 // New convention: < 0 means invalid.
else{
    p.d = -0.1;
}
points.push_back(p);
ROS_INFO("Pose1:\t(%.2f, %.2f)", p.d, p.a);
return points;

} 

list<Polar> QRcodeDetection::findLocalMinima(list<Polar> points){

 // List of local minima that have been found.
list<Polar> localMinima;
 // Whether the last angle's distance was smaller than the current one.
bool lastWasCloser = false;
// The previous point; init to an invalid point.
Polar prev = {-1.0, 0.0};
 for (list<Polar>::iterator i = points.begin(); i != points.end(); i++) {
ROS_INFO("obj:\%.2f", *i);
Polar p = *i;
// If p is a valid point and closer than the previous one.
if (p.d < prev.d) {
// We mark it as a potential candidate for being a local minima.
lastWasCloser = true;
} else {
// Otherwise if i-1 was closer than i-2, i-1 is a local minima.
//if (lastWasCloser) {
localMinima.push_back(p);
//}
lastWasCloser = false;
}
prev = p;
ROS_INFO("Pose2:\t(%.2f)", prev.d);
}
 return localMinima;

}

list<Polar> QRcodeDetection::findObjects(list<Polar> points) {
// List of "object" points: local minima of the scan.
list<Polar> objects;
// Point of the current object's minimum, for setting and adding to objects.
Polar objMin;
Polar last;
last.d = -1.0;
for (list<Polar>::iterator i = points.begin(); i != points.end(); i++) {
ROS_INFO("obj1:\%.2f", *i);
Polar p = *i;
if (p.d >= 0 ){
       if( last.d >= 0){
           if( abs(p.d - last.d) < 0.2){
             if(p.d < objMin.d) {
// if this point is closer than objMin, save it.
               objMin = p;
             }
        }         
 else {
// not in an object; add the previous object to the list and
// make a new one.
  objects.push_back(objMin);
  objMin = p;
 } 
}
else{
 objMin = p;
  }
 } 
else if (last.d >= 0) {
// else if there was an object in progress, add it to the list.
objects.push_back(objMin);
}

last = p;

ROS_INFO("Pose3:\t(%.2f, %.2f)", last.d, p.d);
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
* 5. Add force due to obstacles: updateNetForce
* 6. Convert (x,y) force vector to polar (relative to robot heading)
* 7. Convert force vector to command velocity: cmdTransform
* 8. Check for timeout to reach current waypoint
* 9. Check for recovery (is robot lost) - not sure this check is
* being done properly at present
*/

bool QRcodeDetection::APF(QRcode_detection::Pose qrPose){

std::stringstream ss; 
         QRcode_detection::Goal topicMsg;
if(abs(prePose.x - qrPose.x)>1.0 || abs(prePose.y - qrPose.y)>1.0 || abs(prePose.a - qrPose.theta)>1.0 ){// put the odom pose value in prepose and compare with the qrcode pose
    getRobotPose(robot_pose);
    ROS_DEBUG("clearing ObstacleList");
}
     
 //QRcode_detection::Pose odomPose;
  ROS_INFO("Pose:\t(%lf, %lf), <%lf>", prePose.x, prePose.y, prePose.a);
  // The goal is the head of the seen QRCODE.
  goal.x = qrPose.x;
  goal.y = qrPose.y;
 ROS_DEBUG("Abs Goal:\t(%.4f, %.4f)", goal.x, goal.y);

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
ROS_INFO("Rel Goal:\t(%.4f, %.4f)", goalwrtRobot.x, goalwrtRobot.y);
// The list of "objects" found; already in the robot reference frame.
   list<Polar> objects = QRcodeDetection::findLocalMinima(QRcodeDetection::findObjects(QRcodeDetection::scanobjectList(qrPose)));
     // QRcodeDetection::updateObstacleList(objects);
   
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
           ROS_INFO("GoalF:\t%.4f, %.4f", netForce.x, netForce.y);

   ss << "netForce_X:\n" << netForce.x <<"\n netForce_Y:\n"<< netForce.y << ";";
                           topicMsg.name = ss.str(); 
               obstacle_pub.publish(topicMsg);

                for (list<Polar>::iterator it = objects.begin(); it != objects.end(); ++it) {
                    ROS_INFO("object1:\%.2f", *it);
                    Polar obj = *it;
                    ROS_INFO("hello");
         if (obj.d <= D_OBS) { // if there's any object within the defined window, then turn in place
            // Principles of Robot Motion, pg. 83
                double f = K_OBS * (1.0/D_OBS - 1.0/obj.d) / (obj.d * obj.d);
                   netForce.x += f * cos(obj.a);
                   netForce.y += f * sin(obj.a);
                   ROS_INFO("ObjF:\t%.2f, %.2f\n", f * cos(obj.a), f * sin(obj.a));
}
}
// Resulting command vector. 
    Polar p;
     p.d = length(netForce.x, netForce.y);
    // ROS_INFO("Polar:\t<%+.4f>\n",  p.d);
    if(abs(netForce.y) <= 0.099 && abs(netForce.x) <= 0.099) {
          ROS_INFO("zero net force detected");
              p.a = 0;
                   }
          else {
              p.a = atan2(netForce.y, netForce.x);
                 }
           ROS_INFO("TotalF:\t<%+.4f, %.4f>", p.d, p.a);

   if(p.a > 0.28){

      vel.angular.z = 0.1;

     if(p.a <PI/2.0){

       vel.linear.x = - cos(p.a)*p.d;// we need to check with (-)ve value
     }
  else{
       vel.linear.x =0.0;
 }
}
  else if(p.a < - 0.28){

       vel.angular.z= - 0.05;

    if(p.a > - PI/2.0){
       vel.linear.x = - cos(p.a)*p.d;// we need to check with (-)ve value
}
  else{
        vel.linear.x =0.0;
  } 
 }
  else {
        vel.angular.z = 0.0;
        vel.linear.x = p.d;
}
 if (vel.linear.x >=0.07 && vel.angular.z == 0.0){
       vel.linear.x = 0.05;
       vel.angular.z = 0.1;
}
 else{
        vel.angular.z = 0.0;
        vel.linear.x = p.d;
}

//Wander safely (until we find a barcode and reorient ourselves)
   
   for (list<Polar>::iterator it = objects.begin(); it != objects.end(); ++it) {
          ROS_INFO("object1:\%.2f", *it);
        Polar objWrtRobot = *it;

         if (objWrtRobot.d <= 1 && abs(objWrtRobot.a) <= 0.5) { // if there's any object within the defined window, then turn in place
            vel.linear.x = 0.0;  
            vel.angular.z = 0.1;
   }
    else{
      vel.linear.x = 0.05; 
      vel.angular.z = 0.0;
   }
}
   cmdVelPub.publish(vel);
  
   ROS_INFO("NavVel:\t<%+.4f, %.4f>\n",  vel.linear.x, vel.angular.z);
 return true;
 
}


/*
* Convert the given polar (relative to robot) coordinate to the
* global coordinate system.
*/

Robot::SimplePose* QRcodeDetection::convertRobotToGlobal( Polar &polarPoint){
QRcode_detection::Pose qrPose;
Robot::SimplePose *sp = new Robot::SimplePose();

sp->x = qrPose.x + polarPoint.d * cos(qrPose.theta + polarPoint.a);
sp->y = qrPose.y + polarPoint.d * sin(qrPose.theta + polarPoint.a);
sp->a = 0;
 ROS_INFO("Simple Pose:\t<%.4lf, %.4lf>\n", sp->x, sp->y);
return sp;
}

/**
* Add the given point to the obstacle cache if it is farther than
* OBS_MATCH_DIST away from all other cached obstacles.


bool QRcodeDetection::pushIfUnique(Robot::SimplePose *sp){

CachedPoint newObs;// put the value of robot coordinate system in new obs compare to global
newObs.p.x = sp->x;
newObs.p.y = sp->y;
ObstacleList.push_back(newObs);

//publishing the obstacle's coordinates to the ch_obstacle topic
stringstream ss; 
QRcode_detection::Goal topicMsg;
ss << "(" << newObs.p.x << ", " << newObs.p.y << ")";
topicMsg.name = ss.str(); 
obsPublisher.publish(topicMsg);
return true;
}


double QRcodeDetection::distancefromRobot(Robot::SimplePose &sp){
 QRcode_detection::Pose qrPose;
 double distance = sqrt((sp.x-qrPose.x)*(sp.x-qrPose.x) + (sp.y-qrPose.y)*(sp.y-qrPose.y));
 return distance;
 ROS_INFO_STREAM("distance from Robot:! ----------"<<distance);
}*/

int main(int argc, char **argv)
{
  ros::init(argc, argv, "QRcode_detection");
  QRcodeDetection QRD;
  ros::NodeHandle n;
  ros::Subscriber robotpose_sub = n.subscribe("/RosAria/pose", 1, &getRobotPose);
  pose_pub = n.advertise<Goal>("pose_publish", 10);
  ros::spin();
 
  return 0;
}
