#include "params.hpp"

void params::initialize(std::istream& fileReader) {
    const double MassFactor = constants::fluid_density / ppm / ppm / ppm;
    const double SmoothingFactor = constants::radius_mult / ppm;
    const double DensityFactor1 = SmoothingFactor * SmoothingFactor;
    const double DensityFactor2 = std::pow(SmoothingFactor, 6);
    const double DensityFactor3 = 315.0 * mass / 64.0 / std::numbers::pi / std::pow(SmoothingFactor, 9);
    const double AccFactor1 = SmoothingFactor * SmoothingFactor;
    const double AccFactor2 = 45.0 * mass * constants::stiff_pressure / std::numbers::pi / std::pow(SmoothingFactor, 6) / 2;
    const double AccFactor3 = 45.0 * mass * constants::viscosity / std::numbers::pi / std::pow(SmoothingFactor, 6);

    ppm = read_float(fileReader);
    const int temp_np = read_int(fileReader);

    const int ExitStatusError = 5;

    if (temp_np <= 0) {
        std::cerr << "Error: Invalid number of particles: " << temp_np << ".\n";
        exit(-ExitStatusError);
    }
    np = temp_np;
    mass = MassFactor;
    smoothing_length = SmoothingFactor;
    density_factors = {DensityFactor1, DensityFactor2, DensityFactor3};
    acceleration_factors = {AccFactor1, AccFactor2, AccFactor3};
    grid_size = {
            static_cast<int>(std::floor((constants::max[0] - constants::min[0]) / smoothing_length)),
            static_cast<int>(std::floor((constants::max[1] - constants::min[1]) / smoothing_length)),
            static_cast<int>(std::floor((constants::max[2] - constants::min[2]) / smoothing_length))
    };
    block_size = {
            (constants::max[0] - constants::min[0]) / grid_size[0],
            (constants::max[1] - constants::min[1]) / grid_size[1],
            (constants::max[2] - constants::min[2]) / grid_size[2]
    };
}
