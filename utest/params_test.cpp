#include "../sim/grid.hpp"
#include "../sim/params.hpp"
#include "../sim/progargs.hpp"
#include "../sim/utility.hpp"
#include "utest_utility.hpp"

#include <cstdio>
#include <gtest/gtest.h>

class ParamsTest : public testing::Test {
  public:
    void SetUp() override { }

    void TearDown() override { }

    // Define istream object to pass into initialize()
    params parameters;
    params correct_parameters;
    std::ifstream inFile;
};

TEST_F(ParamsTest, TestInitialize) {
  // Need to test: If temp_np <=0 then exits, try both 0 and negative number
  // If the actual values are what I think they should be
  inFile.open("test-input/small-test-zero-particles.fld", std::ios::binary);
  EXPECT_EXIT(parameters.initialize(inFile), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: 0.\n");
  inFile.close();
  inFile.open("test-input/small-test-negative-particles.fld", std::ios::binary);
  EXPECT_EXIT(parameters.initialize(inFile), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: -1.\n");
  inFile.close();
  inFile.open("reference-input/small.fld", std::ios::binary);
  parameters.initialize(inFile);
  initialize_correct_parameters(correct_parameters);
  EXPECT_EQ(parameters.np, correct_parameters.np);
  EXPECT_EQ(parameters.ppm, correct_parameters.ppm);
  EXPECT_EQ(parameters.mass, correct_parameters.mass);
  EXPECT_EQ(parameters.smoothing_length, correct_parameters.smoothing_length);
}

TEST_F(ParamsTest, TestInitializeFactorsAndVectors) {
  inFile.open("reference-input/small.fld", std::ios::binary);
  // Will initialize basic values then also factors, which we are testing
  parameters.initialize(inFile);
  initialize_correct_parameters(correct_parameters);
  initialize_factors_correct_parameters(correct_parameters);
  initialize_vectors_correct_parameters(correct_parameters);
  EXPECT_EQ(parameters.density_factors, correct_parameters.density_factors);
  EXPECT_EQ(parameters.acceleration_factors, correct_parameters.acceleration_factors);
  EXPECT_EQ(parameters.min, correct_parameters.min);
  EXPECT_EQ(parameters.max, correct_parameters.max);
  EXPECT_EQ(parameters.grid_size, correct_parameters.grid_size);
  EXPECT_EQ(parameters.block_size, correct_parameters.block_size);
}