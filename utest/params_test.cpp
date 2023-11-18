#include "../sim/params.hpp"
#include <gtest/gtest.h>

class ParamsTest : public testing::Test {
  public:
    void SetUp() override { }
    void TearDown() override { }

    std::ifstream inFile;
    params parameters;
    std::vector<double> const min = {-0.065, -0.08, -0.065};
    std::vector<double> const max = {0.065, 0.1, 0.065};

    int const small_np  = 4800;
    int const small_ppm = 204;
    double const small_mass = constants::fluid_density / small_ppm / small_ppm / small_ppm;
    double const small_smoothing_length = constants::radius_mult / small_ppm;
    std::vector<int> const small_gsize = {15, 21, 15};
    std::vector<double> const small_bsize = {(max[0] - min[0]) / small_gsize[0],
                                             (max[1] - min[1]) / small_gsize[1],
                                             (max[2] - min[2]) / small_gsize[2]};
    int const large_np = 15138;
    int const large_ppm = 292;
    double const large_mass = constants::fluid_density / large_ppm / large_ppm / large_ppm;
    double const large_smoothing_length = constants::radius_mult / large_ppm;
    std::vector<int> const large_gsize = {22, 31, 22};
    std::vector<double> const large_bsize = {(max[0] - min[0]) / large_gsize[0],
                                             (max[1] - min[1]) / large_gsize[1],
                                             (max[2] - min[2]) / large_gsize[2]};
};

TEST_F(ParamsTest, TestInitializeInvalid) {
  inFile.open("test-input/small-test-zero-particles.fld", std::ios::binary);
  EXPECT_EXIT(parameters.initialize(inFile), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: 0.\n");
  inFile.close();
  inFile.open("test-input/small-test-negative-particles.fld", std::ios::binary);
  EXPECT_EXIT(parameters.initialize(inFile), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: -1.\n");
  inFile.close();
}

TEST_F(ParamsTest, TestInitializeSmall) {
  inFile.open("reference-input/small.fld", std::ios::binary);
  parameters.initialize(inFile);
  inFile.close();

  EXPECT_EQ(parameters.np, small_np);
  EXPECT_EQ(parameters.ppm, small_ppm);
  EXPECT_EQ(parameters.mass, small_mass);
  EXPECT_EQ(parameters.smoothing_length, small_smoothing_length);
  EXPECT_EQ(parameters.min, min);
  EXPECT_EQ(parameters.max, max);
  EXPECT_EQ(parameters.grid_size, small_gsize);
  EXPECT_EQ(parameters.block_size, small_bsize);
}

TEST_F(ParamsTest, TestInitializeLarge) {
  inFile.open("reference-input/large.fld", std::ios::binary);
  parameters.initialize(inFile);
  inFile.close();

  EXPECT_EQ(parameters.np, large_np);
  EXPECT_EQ(parameters.ppm, large_ppm);
  EXPECT_EQ(parameters.mass, large_mass);
  EXPECT_EQ(parameters.smoothing_length, large_smoothing_length);
  EXPECT_EQ(parameters.min, min);
  EXPECT_EQ(parameters.max, max);
  EXPECT_EQ(parameters.grid_size, large_gsize);
  EXPECT_EQ(parameters.block_size, large_bsize);
}