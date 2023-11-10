add_executable(utest
        grid_test.cpp
        progargs_test.cpp
        block_test.cpp
)


target_include_directories(utest PRIVATE ../sim)


target_link_libraries(utest
        PRIVATE
        sim
        GTest::gtest_main
        Microsoft.GSL::GSL
)

include(GoogleTest)
gtest_discover_tests(utest)