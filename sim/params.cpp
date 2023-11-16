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
  initializeFactors();
}

void params::initializeFactors() {
  int const mag1    = 6;
  double const mag2 = 315;
  double const mag3 = 64;
  int const mag4    = 9;
  double const mag5 = 45;
  density_factors = {smoothing_length * smoothing_length, std::pow(smoothing_length, mag1), mag2 *
    mass / mag3 / std::numbers::pi / std::pow(smoothing_length, mag4)};
  acceleration_factors = {smoothing_length * smoothing_length, mag5 * mass * 
    constants::stiff_pressure / std::numbers::pi / std::pow(smoothing_length, mag1) / 2, mag5 * 
    mass * constants::viscosity / std::numbers::pi / std::pow(smoothing_length, mag1)};
}

void params::initializeVectors() {
  double const mag1 = -0.065;
  double const mag2 = -0.08;
  double const mag3 = 0.1;
  min = {mag1, mag2, mag1};
  max = {-mag1, mag3, -mag1};
  grid_size            = {
    static_cast<int>(std::floor((max[0] - min[0]) / smoothing_length)),
    static_cast<int>(std::floor((max[1] - min[1]) / smoothing_length)),
    static_cast<int>(std::floor((max[2] - min[2]) / smoothing_length))};
  block_size = {(max[0] - min[0]) / grid_size[0],
                (max[1] - min[1]) / grid_size[1],
                (max[2] - min[2]) / grid_size[2]};
}
