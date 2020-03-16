file(REMOVE_RECURSE
  "libhippmpi.pdb"
  "libhippmpi.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/hippmpi.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
