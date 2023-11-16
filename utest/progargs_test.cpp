#include <gtest/gtest.h>
#include <cstdio>
#include "../sim/progargs.hpp"

class ProgargsTest : public testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ProgargsTest, checkArgNumber_Test) {
    EXPECT_EXIT(checkArgNumber(1), testing::ExitedWithCode(255), 
    "Error: Invalid number of arguments: 0.\n");

    EXPECT_EXIT(checkArgNumber(3), testing::ExitedWithCode(255), 
    "Error: Invalid number of arguments: 2.\n");

    EXPECT_EXIT(checkArgNumber(10), testing::ExitedWithCode(255), 
    "Error: Invalid number of arguments: 9.\n");

    EXPECT_EXIT( { checkArgNumber(4);
                   std::cerr << "function did not exit";
                   exit(0); 
                 }, testing::ExitedWithCode(0), "function did not exit");
}

