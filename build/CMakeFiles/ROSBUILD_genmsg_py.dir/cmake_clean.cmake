FILE(REMOVE_RECURSE
  "../msg_gen"
  "../srv_gen"
  "../msg_gen"
  "../srv_gen"
  "../src/QRcode_detection/msg"
  "../src/QRcode_detection/srv"
  "CMakeFiles/ROSBUILD_genmsg_py"
  "../src/QRcode_detection/msg/__init__.py"
  "../src/QRcode_detection/msg/_UIConfirm.py"
  "../src/QRcode_detection/msg/_Pose.py"
  "../src/QRcode_detection/msg/_Markers.py"
  "../src/QRcode_detection/msg/_Goal.py"
  "../src/QRcode_detection/msg/_UIMessage.py"
  "../src/QRcode_detection/msg/_Landmark.py"
  "../src/QRcode_detection/msg/_PerceptInfo.py"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/ROSBUILD_genmsg_py.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
