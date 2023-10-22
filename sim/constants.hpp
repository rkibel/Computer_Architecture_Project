#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <vector>

namespace constants {
    const double radius_mult = 1.695;
    const double fluid_density = 1e3;
    const double stiff_pressure = 3.0;
    const double stiff_collisions = 3e4;
    const double damping = 128.0;
    const double viscosity = 0.4;
    const double particle_size = 2e-4;
    const double delt_t = 1e-3;
    const std::vector<double> acceleration{0.0, -9.8, 0.0};
    const std::vector<double> min{-0.065, -0.08, -0.065};
    const std::vector<double> max{0.065, 0.1, 0.065};
}

#endif // CONSTANTS_HPP
