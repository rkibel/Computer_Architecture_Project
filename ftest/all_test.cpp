#include <gtest/gtest.h>
#include <cstdio>
#include "../sim/grid.hpp"
#include "../sim/progargs.hpp"
#include "../sim/utility.hpp"

class FTest : public testing::Test {
    public:
    void SetUp() override {}
    void TearDown() override {}

    grid small = parseInputFile("reference-input/small.fld");
    grid large = parseInputFile("reference-input/large.fld");
};

TEST_F(FTest, TestSmall) {
    const int step_count = 5;
    for (int i = 0; i < step_count; ++i) {
        small.processStep();
        const auto read_ppm = static_cast<float>(small.parameters.ppm);
        const auto read_np = static_cast<int>(small.parameters.np);
        const std::string file_name = "small-" + std::to_string(i+1) + ".fld";
        writeFile("ftest/" + file_name, read_ppm, read_np, small.part_dict);
        EXPECT_TRUE(compare_binary_files("ftest/" + file_name, "reference-output/" + file_name));
        EXPECT_EQ(remove(("ftest/" + file_name).c_str()), 0);
    }
}

TEST_F(FTest, TestLarge) {
    const int step_count = 5;
    for (int i = 0; i < step_count; ++i) {
        large.processStep();
        const auto read_ppm = static_cast<float>(large.parameters.ppm);
        const auto read_np = static_cast<int>(large.parameters.np);
        const std::string file_name = "large-" + std::to_string(i+1) + ".fld";
        writeFile("ftest/" + file_name, read_ppm, read_np, large.part_dict);
        EXPECT_TRUE(compare_binary_files("ftest/" + file_name, "reference-output/" + file_name));
        EXPECT_EQ(remove(("ftest/" + file_name).c_str()), 0);
    }
}