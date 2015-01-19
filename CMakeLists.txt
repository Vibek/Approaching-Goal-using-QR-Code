cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)
set(ROS_BUILD_TYPE Debug)

rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)

#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(example examples/example.cpp)
#target_link_libraries(example ${PROJECT_NAME})
#find_package(pcl_conversions pcl_ros REQUIRED)
#find_package(markers_msgs REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})

rosbuild_add_executable(QRcode_detection src/QRcode_detection.cpp)
rosbuild_add_executable(QRcode_detection src/QRcode_detection.h)
rosbuild_add_executable(QRcode_detection src/CSVReader.cpp)
rosbuild_add_executable(QRcode_detection src/Robot.cpp)
#rosbuild_add_library(QRcode_detection src/Robot.cpp)
#rosbuild_add_executable(tinyxml src/tinyxml.h)
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
find_package(OpenCV REQUIRED)
target_link_libraries(QRcode_detection ${OpenCV_LIBS})
# Find PCL
find_package(PCL REQUIRED)
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})
target_link_libraries(QRcode_detection ${PCL_LIBRARIES})
target_link_libraries(QRcode_detection -lzbar)

#add_custom_command(
#OUTPUT lib/libzbar.so
#COMMAND make INSTALL_DIR=${PROJECT_SOURCE_DIR} CFLAGS=""
#WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/3rdparty/zbar
#)

#add_custom_target(
#zbar_built
#DEPENDS lib/libzbar.so
#)

#include_directories(include)
#add_library(zbar SHARED IMPORTED)
#add_dependencies(zbar zbar_built)
#set_target_properties(zbar PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/libzbar.so)


#TODO: Define non-absolute include path
#Teporary includes of opencv with sse3 disabled
#target_link_libraries(${PROJECT_NAME} /home/konstantin/opencv/opencv2.3_bin/lib/libopencv_highgui.so)
#target_link_libraries(${PROJECT_NAME} /home/konstantin/opencv/opencv2.3_bin/lib/libopencv_imgproc.so)
#target_link_libraries(${PROJECT_NAME} /home/konstantin/opencv/opencv2.3_bin/lib/libopencv_features2d.so)

