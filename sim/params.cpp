#include "params.hpp"

void params::initialize(std::istream & fileReader) {
  int const exit_status_error = -5;                      // Magic numbers
  ppm                         = read_float(fileReader);  // Reading
  int const temp_np           = read_int(fileReader);
  if (temp_np <= 0) {
    std::cerr << "Error: Invalid number of particles: " << temp_np << ".\n";
    exit(exit_status_error);
  }
  np               = temp_np;
  mass             = constants::fluid_density / ppm / ppm / ppm;
  smoothing_length = constants::radius_mult / ppm;
  initializeVectors();
}

void params::initializeVectors() {
  int const magic_1    = 6;
  double const magic_2 = 315.0;
  double const magic_3 = 64.0;
  int const magic_4    = 9;
  double const magic_5 = 45.0;
  density_factors      = {smoothing_length * smoothing_length, std::pow(smoothing_length, magic_1),
                          magic_2 * mass / magic_3 / std::numbers::pi /
                              std::pow(smoothing_length, magic_4)};
  acceleration_factors = {smoothing_length * smoothing_length,
                          magic_5 * mass * constants::stiff_pressure / std::numbers::pi /
                              std::pow(smoothing_length, magic_1) / 2,
                          magic_5 * mass * constants::viscosity / std::numbers::pi /
                              std::pow(smoothing_length, magic_1)};
  grid_size            = {
    static_cast<int>(std::floor((constants::max[0] - constants::min[0]) / smoothing_length)),
    static_cast<int>(std::floor((constants::max[1] - constants::min[1]) / smoothing_length)),
    static_cast<int>(std::floor((constants::max[2] - constants::min[2]) / smoothing_length))};
  block_size = {(constants::max[0] - constants::min[0]) / grid_size[0],
                (constants::max[1] - constants::min[1]) / grid_size[1],
                (constants::max[2] - constants::min[2]) / grid_size[2]};
}
