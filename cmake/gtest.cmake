add_library(gtest-interface-lib INTERFACE)
target_link_libraries(gtest-interface-lib 
    INTERFACE -lgtest -lgtest_main)

