FILE(REMOVE_RECURSE
  "../msg_gen"
  "../srv_gen"
  "../msg_gen"
  "../srv_gen"
  "../src/QRcode_detection/msg"
  "../src/QRcode_detection/srv"
  "CMakeFiles/ROSBUILD_gensrv_py"
  "../src/QRcode_detection/srv/__init__.py"
  "../src/QRcode_detection/srv/_GetPixelOccupancy.py"
  "../src/QRcode_detection/srv/_GetLandmark.py"
  "../src/QRcode_detection/srv/_GetLandmarks.py"
  "../src/QRcode_detection/srv/_GetNeighbors.py"
  "../src/QRcode_detection/srv/_GetCoMap.py"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/ROSBUILD_gensrv_py.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
