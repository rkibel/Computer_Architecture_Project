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

TEST_F(ProgargsTest, parseInt_Test) {
    EXPECT_EXIT(parseInt("This won't work"), testing::ExitedWithCode(255),
                "Error: time steps must be numeric.\n");

    EXPECT_EXIT(parseInt(""), testing::ExitedWithCode(255),
                "Error: time steps must be numeric.\n");

    EXPECT_EXIT(parseInt("31415926535897942881273403249231843824914892318491258021753"), testing::ExitedWithCode(255),
                "Error: time steps must be numeric.\n");

    EXPECT_EXIT(parseInt("3.1415"), testing::ExitedWithCode(255),
                "Error: time steps must be numeric.\n");

    EXPECT_EXIT(parseInt("-1"), testing::ExitedWithCode(254),
                "Error: Invalid number of time steps.\n");

    EXPECT_EXIT( { parseInt("100");
                     std::cerr << "function did not exit";
                     exit(0);
                 }, testing::ExitedWithCode(0), "function did not exit");
}