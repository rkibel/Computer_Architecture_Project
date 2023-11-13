# Executable for all unit tests with list of sources
# For example, you may have one *_test.cpp for each *.cpp in sim
add_executable(ftest all_test.cpp)

target_include_directories(ftest PRIVATE ../sim)


target_link_libraries(ftest
        PRIVATE
        sim
        GTest::gtest_main
        Microsoft.GSL::GSL
)

include(GoogleTest)
gtest_discover_tests(ftest)