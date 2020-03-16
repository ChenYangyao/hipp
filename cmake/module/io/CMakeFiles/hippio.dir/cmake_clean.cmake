file(REMOVE_RECURSE
  "libhippio.pdb"
  "libhippio.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/hippio.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
