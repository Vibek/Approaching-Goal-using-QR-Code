FILE(REMOVE_RECURSE
  "CMakeFiles/zbar_built"
  "lib/libzbar.so"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/zbar_built.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
