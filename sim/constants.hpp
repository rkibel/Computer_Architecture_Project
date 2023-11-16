#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <vector>

namespace constants {
  double const radius_mult      = 1.695;
  double const fluid_density    = 1e3;
  double const stiff_pressure   = 3.0;
  double const stiff_collisions = 3e4;
  double const damping          = 128.0;
  double const viscosity        = 0.4;
  double const particle_size    = 2e-4;
  double const delt_t           = 1e-3;
  double const grav             = -9.8;
}  // namespace constants

#endif  // CONSTANTS_HPP
