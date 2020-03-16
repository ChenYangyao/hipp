file(REMOVE_RECURSE
  "libhippcntl.pdb"
  "libhippcntl.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/hippcntl.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
