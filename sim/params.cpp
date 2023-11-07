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
  initializeVectors1();
  initializeVectors2();
}

void params::initializeVectors1() {
  double const magic_6 = -9.8;
  double const magic_7 = -0.065;
  double const magic_8 = -0.08;
  double const magic_9 = 0.1;

  acceleration = {0.0, magic_6, 0.0};
  min          = {magic_7, magic_8, magic_7};
  max          = {-magic_7, magic_9, -magic_7};
}

void params::initializeVectors2() {
  int const mag1    = 6;
  double const mag2 = 315.0;
  double const mag3 = 64.0;
  int const mag4    = 9;
  double const mag5 = 45.0;

  density_factors      = {smoothing_length * smoothing_length, std::pow(smoothing_length, mag1),
                          mag2 * mass / mag3 / std::numbers::pi / std::pow(smoothing_length, mag4)};
  acceleration_factors = {smoothing_length * smoothing_length,
                          mag5 * mass * constants::stiff_pressure / std::numbers::pi /
                              std::pow(smoothing_length, mag1) / 2,
                          mag5 * mass * constants::viscosity / std::numbers::pi /
                              std::pow(smoothing_length, mag1)};
  for (int i = 0; i < 3; i++) {
    grid_size.push_back(static_cast<int>(std::floor((max[i] - min[i]) / smoothing_length)));
    block_size.push_back((max[i] - min[i]) / grid_size[i]);
  }
}
