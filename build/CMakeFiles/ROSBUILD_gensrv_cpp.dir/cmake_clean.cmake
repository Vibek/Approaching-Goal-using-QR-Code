FILE(REMOVE_RECURSE
  "../msg_gen"
  "../srv_gen"
  "../msg_gen"
  "../srv_gen"
  "../src/QRcode_detection/msg"
  "../src/QRcode_detection/srv"
  "CMakeFiles/ROSBUILD_gensrv_cpp"
  "../srv_gen/cpp/include/QRcode_detection/GetPixelOccupancy.h"
  "../srv_gen/cpp/include/QRcode_detection/GetLandmark.h"
  "../srv_gen/cpp/include/QRcode_detection/GetLandmarks.h"
  "../srv_gen/cpp/include/QRcode_detection/GetNeighbors.h"
  "../srv_gen/cpp/include/QRcode_detection/GetCoMap.h"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/ROSBUILD_gensrv_cpp.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
