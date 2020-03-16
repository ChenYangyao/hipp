file(REMOVE_RECURSE
  "libhippnumerical.pdb"
  "libhippnumerical.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/hippnumerical.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
