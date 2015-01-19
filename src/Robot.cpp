#include <cmath>
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Quaternion.h"
#include "QRcode_detection/Pose.h"
#include "QRcode_detection.h"

namespace Robot {

double length(double x, double y) {
        return sqrt(x * x + y * y);
    }

geometry_msgs::Point rCoordTransform(geometry_msgs::Point aPoint, QRcode_detection::Pose bOrigin) {
geometry_msgs::Point bPoint;
float dx = aPoint.x - bOrigin.x;
float dy = aPoint.y - bOrigin.y;
float theta = -bOrigin.theta;
bPoint.x = dx * cos(theta) - dy * sin(theta);
bPoint.y = dx * sin(theta) + dy * cos(theta);
return bPoint;
}
double bound(double n, double b, double r){

if (n > b + r) {
return b + r;
} else if (n < b - r) {
return b - r;
} else {
return n;
}
}
 }
