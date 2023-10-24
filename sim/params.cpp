#include "params.hpp"

void params::initialize(std::istream& fileReader) {
    ppm = read_float(fileReader);
    int temp_np = read_int(fileReader);
    if (temp_np <= 0) {
        std::cerr << "Error: Invalid number of particles: " << temp_np << ".\n";
        exit(-5);
    }
    np = temp_np;
    mass = constants::fluid_density / ppm / ppm / ppm;
    smoothing_length = constants::radius_mult / ppm;
    density_factors = {
        smoothing_length*smoothing_length, 
        std::pow(smoothing_length, 6), 
        315.0 * mass / 64.0 / std::numbers::pi / std::pow(smoothing_length, 9)
    };
    acceleration_factors = {
        smoothing_length * smoothing_length,
        45.0 * mass * constants::stiff_pressure / std::numbers::pi / std::pow(smoothing_length, 6) / 2,
        45.0 * mass * constants::viscosity / std::numbers::pi / std::pow(smoothing_length, 6)
    };
    grid_size = {static_cast<int>(std::floor((constants::max[0] - constants::min[0]) / smoothing_length)), 
                 static_cast<int>(std::floor((constants::max[1] - constants::min[1]) / smoothing_length)),
                 static_cast<int>(std::floor((constants::max[2] - constants::min[2]) / smoothing_length))};
    block_size = {(constants::max[0] - constants::min[0]) / grid_size[0], 
                  (constants::max[1] - constants::min[1]) / grid_size[1],
                  (constants::max[2] - constants::min[2]) / grid_size[2]};
}