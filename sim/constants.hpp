#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <vector>

namespace constants {
  double const radius_mult               = 1.695;
  double const fluid_density             = 1e3;
  double const stiff_pressure            = 3.0;
  double const stiff_collisions          = 3e4;
  double const damping                   = 128.0;
  double const viscosity                 = 0.4;
  double const particle_size             = 2e-4;
  double const delt_t                    = 1e-3;
  std::vector<double> const acceleration = {0.0, -9.8, 0.0};
  std::vector<double> const min          = {-0.065, -0.08, -0.065};
  std::vector<double> const max          = {0.065, 0.1, 0.065};
}  // namespace constants

#endif  // CONSTANTS_HPP
