FILE(REMOVE_RECURSE
  "../msg_gen"
  "../srv_gen"
  "../msg_gen"
  "../srv_gen"
  "../src/QRcode_detection/msg"
  "../src/QRcode_detection/srv"
  "CMakeFiles/ROSBUILD_genmsg_cpp"
  "../msg_gen/cpp/include/QRcode_detection/UIConfirm.h"
  "../msg_gen/cpp/include/QRcode_detection/Pose.h"
  "../msg_gen/cpp/include/QRcode_detection/Markers.h"
  "../msg_gen/cpp/include/QRcode_detection/Goal.h"
  "../msg_gen/cpp/include/QRcode_detection/UIMessage.h"
  "../msg_gen/cpp/include/QRcode_detection/Landmark.h"
  "../msg_gen/cpp/include/QRcode_detection/PerceptInfo.h"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/ROSBUILD_genmsg_cpp.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
