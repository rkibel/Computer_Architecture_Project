// constants.hpp
#include <vector>

namespace constants {
  double const radius_mult      = 1.695;
  double const fluid_density    = 1000;
  double const stiff_pressure   = 3.0;
  double const stiff_collisions = 3E4;
  double const damping          = 128.0;
  double const viscosity        = 0.4;
  double const particle_size    = 2E-4;
  double const delt_t           = 1E-3;
  const std::vector<double> acceleration{0.0, -9.8, 0.0};
  const std::vector<double> min{-0.065, -0.08, -0.065};
  const std::vector<double> max{0.065, 0.1, 0.065};
}  // namespace constants