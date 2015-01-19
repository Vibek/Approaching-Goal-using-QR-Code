//=================================================================================================
// Copyright (c) 2014, Vibekananda Dutta, WUT
// All rights reserved.

//=================================================================================================
#ifndef HECTOR_QRCODE_DETECTION_H
#define HECTOR_QRCODE_DETECTION_H

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <opencv/cv.h>
#include <stdio.h>
#include <ros/console.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/image_encodings.h>
#include <time.h>
#include <tf/transform_listener.h>
#include <sstream>
#include <zbar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>  
#include <string.h>  
#include <sstream>
#include <geometry_msgs/Point.h>
#include <iostream>
#include <tf/transform_listener.h>
#include <QRcode_detection/Pose.h>
#include <QRcode_detection/Goal.h>
#include <opencv2/core/core.hpp>
#include <list>
#include <nav_msgs/Odometry.h>
#include "CSVReader.h"
#include "QRcode_detection/Markers.h"

//#include <opencv/cv.h>
#define PI 3.14159265

// distance from camera to center of robot
#define CAMERA_OFFSET 0.16 

/** The distance at which to switch from conical to quadratic goal attraction. */
#define D_GOAL 0.27

/** The distance at which to start paying attention to obstacles. */
#define D_OBS 1.3

/** Goal gain (constant factor). */
#define K_GOAL 0.37

/** Obstacle gain (constant factor). */
#define K_OBS 0.7

/** Threshold distance for matching an obstacle in the cache. */
#define OBS_MATCH_DIST 0.5

// distance from camera to center of robot
#define CAMERA_OFFSET 1.5 


using geometry_msgs::Point;
using namespace std;

namespace zbar {
  class ImageScanner;
}

 /**
* As simple as a pose gets.
*/
namespace Robot{

  typedef struct {
double x;
double y;
double a;
}SimplePose;

  /**
* Calculates the length of the vector <x, y>.
*
* @param x The x coordinate.
* @param y The y coordinate.
* @return The straight-line distance from (0, 0) to (x, y).
*/
double length(double x, double y);
double bound(double n, double b, double r);

/**
* Transforms a pose from one reference frame to another.
*
* @param aPose The pose in frame A.
* @param aOrigin The origin of frame A in frame B.
* @return The pose converted into frame B.
*/
 geometry_msgs::Point rCoordTransform(geometry_msgs::Point aPoint, QRcode_detection::Pose bOrigin);
}

typedef struct {
Point p;
//double lastT;
} CachedPoint;

typedef struct {
/** Distance (radius) in meters. */
float d;
/** Angle in radians. */
float a;

} Polar;


typedef struct {
int x, y;
} Pt;


class QRcodeDetection{
public:
    QRcodeDetection();
    ~QRcodeDetection();
     bool localization_from_markers_updated, laser_update;
     string csvfile;
     string csvfilename;
     CSVReader csvreader;
     QRcode_detection::Pose seenQRPose; 
     QRcode_detection::Pose goal;
     QRcode_detection::Pose odomPose;
     QRcode_detection::Pose goalwrtRobot;
     QRcode_detection::Pose percept;
     nav_msgs::Odometry robot_pose;
     bool checkIfNewQR(QRcode_detection::Pose qrPose);
     double distancefromRobot(Robot::SimplePose &sp);
     bool APF(QRcode_detection::Pose qrPose);
     Pt point[4];
          
 private:

    void imageCallback(const sensor_msgs::ImageConstPtr& img); 
    //Polar* convertFromGlobalToRobotInPolar(Point &sp);
    Robot::SimplePose* convertRobotToGlobal(Polar &p);
    list<Polar> scanobjectList(QRcode_detection::Pose& odomPose);
    list<Polar> findLocalMinima(list<Polar> points);
    list<Polar> findObjects(list<Polar> points);
    void updateObstacleList(list<Polar>& objects);
    bool pushIfUnique(Robot::SimplePose *sp);
    ros::Publisher obsPublisher;
    ros::Publisher cmdVelPub;
    ros::Subscriber robotpose_sub;
    ros::Publisher percept_publisher_;
    ros::Publisher qrCodeCountPublisher;
    ros::Publisher obstacle_pub;
    image_transport::CameraSubscriber camera_sub_;
    image_transport::CameraPublisher rotated_image_pub_;
    image_transport::CameraPublisher qrcode_image_pub_;
    ros::Publisher pose_pub;
    image_transport::Subscriber image_sub_;
    Robot::SimplePose prePose;
    
    zbar::ImageScanner *scanner_;
  
    int qrCount;
    int lengthPixelL, lengthPixelR;
    float distanceL, distanceR, squareDistanceL, squareDistanceR, distanceAvg, angleR, angleL, angleAvg, offsetDistance,  barcodeXavg ;
    float cbx, cby, cbtheta, bcx, bcy, bctheta, alpha, gamma, angle;
    float qrcodeX, qrcodeY;
     float barcodeX, barcodeY;
    int rotation_image_size_;
    string orientation;
    
    std::vector<CachedPoint> ObstacleList;
    
};

#endif // HECTOR_QRCODE_DETECTION_H
