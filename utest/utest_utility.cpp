#include "utest_utility.hpp"

void initialize_correct_parameters(params & param) {  // Basing off small.txt
  int const small_fld_np  = 4800;
  int const small_fld_ppm = 204;
  param.np                = small_fld_np;
  param.ppm               = small_fld_ppm;
  param.mass              = constants::fluid_density / param.ppm / param.ppm / param.ppm;
  param.smoothing_length  = constants::radius_mult / param.ppm;
}

void initialize_factors_correct_parameters(params & param) {
  int const mag1        = 6;
  double const mag2     = 315;
  double const mag3     = 64;
  int const mag4        = 9;
  double const mag5     = 45;
  param.density_factors = {
    param.smoothing_length * param.smoothing_length, std::pow(param.smoothing_length, mag1),
    mag2 * param.mass / mag3 / std::numbers::pi / std::pow(param.smoothing_length, mag4)};
  param.acceleration_factors = {param.smoothing_length * param.smoothing_length,
                                mag5 * param.mass * constants::stiff_pressure / std::numbers::pi /
                                    std::pow(param.smoothing_length, mag1) / 2,
                                mag5 * param.mass * constants::viscosity / std::numbers::pi /
                                    std::pow(param.smoothing_length, mag1)};
}

void initialize_vectors_correct_parameters(params & param) {
  double const mag1 = -0.065;
  double const mag2 = -0.08;
  double const mag3 = 0.1;
  param.min         = {mag1, mag2, mag1};
  param.max         = {-mag1, mag3, -mag1};
  param.grid_size   = {
    static_cast<int>(std::floor((param.max[0] - param.min[0]) / param.smoothing_length)),
    static_cast<int>(std::floor((param.max[1] - param.min[1]) / param.smoothing_length)),
    static_cast<int>(std::floor((param.max[2] - param.min[2]) / param.smoothing_length))};
  param.block_size = {(param.max[0] - param.min[0]) / param.grid_size[0],
                      (param.max[1] - param.min[1]) / param.grid_size[1],
                      (param.max[2] - param.min[2]) / param.grid_size[2]};
}