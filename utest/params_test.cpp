#include "../sim/grid.hpp"
#include "../sim/params.hpp"
#include "../sim/progargs.hpp"
#include "../sim/utility.hpp"

#include <cstdio>
#include <gtest/gtest.h>

class ParamsTest : public testing::Test {
  public:
    void SetUp() override { }

    void TearDown() override { }

    // Define istream object to pass into initialize()
    void initialize_correct_parameters();
    void initialize_factors_correct_parameters();
    void initialize_vectors_correct_parameters();
    params parameters;
    params correct_parameters;
    std::ifstream inFile;
};

void ParamsTest::initialize_correct_parameters() {  // Basing off small.txt
  int const small_fld_np  = 4800;
  int const small_fld_ppm = 204;
  correct_parameters.np   = small_fld_np;
  correct_parameters.ppm  = small_fld_ppm;
  correct_parameters.mass = constants::fluid_density / correct_parameters.ppm /
                            correct_parameters.ppm / correct_parameters.ppm;
  correct_parameters.smoothing_length = constants::radius_mult / correct_parameters.ppm;
}

void ParamsTest::initialize_factors_correct_parameters() {
  int const mag1                          = 6;
  double const mag2                       = 315;
  double const mag3                       = 64;
  int const mag4                          = 9;
  double const mag5                       = 45;
  correct_parameters.density_factors      = {correct_parameters.smoothing_length *
                                                 correct_parameters.smoothing_length,
                                             std::pow(correct_parameters.smoothing_length, mag1),
                                             mag2 * correct_parameters.mass / mag3 / std::numbers::pi /
                                                 std::pow(correct_parameters.smoothing_length, mag4)};
  correct_parameters.acceleration_factors = {
    correct_parameters.smoothing_length * correct_parameters.smoothing_length,
    mag5 * correct_parameters.mass * constants::stiff_pressure / std::numbers::pi /
        std::pow(correct_parameters.smoothing_length, mag1) / 2,
    mag5 * correct_parameters.mass * constants::viscosity / std::numbers::pi /
        std::pow(correct_parameters.smoothing_length, mag1)};
}

void ParamsTest::initialize_vectors_correct_parameters() {
  double const mag1            = -0.065;
  double const mag2            = -0.08;
  double const mag3            = 0.1;
  correct_parameters.min       = {mag1, mag2, mag1};
  correct_parameters.max       = {-mag1, mag3, -mag1};
  correct_parameters.grid_size = {
    static_cast<int>(std::floor((correct_parameters.max[0] - correct_parameters.min[0]) /
                                correct_parameters.smoothing_length)),
    static_cast<int>(std::floor((correct_parameters.max[1] - correct_parameters.min[1]) /
                                correct_parameters.smoothing_length)),
    static_cast<int>(std::floor((correct_parameters.max[2] - correct_parameters.min[2]) /
                                correct_parameters.smoothing_length))};
  correct_parameters.block_size = {
    (correct_parameters.max[0] - correct_parameters.min[0]) / correct_parameters.grid_size[0],
    (correct_parameters.max[1] - correct_parameters.min[1]) / correct_parameters.grid_size[1],
    (correct_parameters.max[2] - correct_parameters.min[2]) / correct_parameters.grid_size[2]};
}

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
  initialize_correct_parameters();
  EXPECT_EQ(parameters.np, correct_parameters.np);
  EXPECT_EQ(parameters.ppm, correct_parameters.ppm);
  EXPECT_EQ(parameters.mass, correct_parameters.mass);
  EXPECT_EQ(parameters.smoothing_length, correct_parameters.smoothing_length);
}

TEST_F(ParamsTest, TestInitializeFactorsAndVectors) {
  inFile.open("reference-input/small.fld", std::ios::binary);
  // Will initialize basic values then also factors, which we are testing
  parameters.initialize(inFile);
  initialize_correct_parameters();
  initialize_factors_correct_parameters();
  initialize_vectors_correct_parameters();
  EXPECT_EQ(parameters.density_factors, correct_parameters.density_factors);
  EXPECT_EQ(parameters.acceleration_factors, correct_parameters.acceleration_factors);
  EXPECT_EQ(parameters.min, correct_parameters.min);
  EXPECT_EQ(parameters.max, correct_parameters.max);
  EXPECT_EQ(parameters.grid_size, correct_parameters.grid_size);
  EXPECT_EQ(parameters.block_size, correct_parameters.block_size);
}