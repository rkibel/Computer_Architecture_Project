#include "grid.hpp"

grid::grid(std::istream & fileReader) : ppm(read_float(fileReader)) {
  int const temp_np = read_int(fileReader);
  if (temp_np <= 0) {
    std::cerr << "Error: Invalid number of particles: " << temp_np << ".\n";
    int const exit_status_error = -5;
    exit(exit_status_error);
  }
  np = temp_np;
  mass = constants::fluid_density / ppm / ppm / ppm;
  smoothing_length = constants::radius_mult / ppm;
  initializeVectors();
  initializeFactors();
  initializeParticles(fileReader);
}

void grid::initializeVectors() {
  double const mag1 = -9.8;
  double const mag2 = -0.065;
  double const mag3 = -0.08;
  double const mag4 = 0.1;
  acceleration = {0.0, mag1, 0.0};
  min = {mag2, mag3, mag2};
  max = {-mag2, mag4, -mag2};
  for (int i = 0; i < 3; ++i) { 
    const int temp = static_cast<int>(std::floor((max[i] - min[i]) / smoothing_length));
    grid_size.push_back(temp);
    block_size.push_back((max[i] - min[i]) / temp);
  }
}

void grid::initializeFactors() {
  int const mag1    = 6;
  double const mag2 = 315.0;
  double const mag3 = 64.0;
  int const mag4    = 9;
  double const mag5 = 45.0;
  density_factors = {smoothing_length * smoothing_length, std::pow(smoothing_length, mag1),
  mag2 * mass / mag3 / std::numbers::pi / std::pow(smoothing_length, mag4)};
  acceleration_factors = {smoothing_length * smoothing_length, mag5 * mass * 
  constants::stiff_pressure / std::numbers::pi / std::pow(smoothing_length, mag1) / 2, mag5 * 
  mass * constants::viscosity / std::numbers::pi / std::pow(smoothing_length, mag1)};
}

void grid::initializeParticles(std::istream & fileReader) {
  int counter = 0;
  while (!fileReader.eof()) {
    particle part;
    part.id         = counter;
    int const three = 3;
    for (int i = 0; i < three; ++i) { part.position.push_back(read_float(fileReader)); }
    for (int i = 0; i < three; ++i) { part.boundary.push_back(read_float(fileReader)); }
    for (int i = 0; i < three; ++i) { part.velocity.push_back(read_float(fileReader)); }
    part_dict.push_back(part);
    counter++;
  }
  part_dict.pop_back();
  initializeNeighborCombinations();  // initializing neighbor combinations
}

void grid::addNeighborCombination(std::vector<std::vector<int>> & neighbor_combinations,
                                  std::vector<int> const & neighbor_pos) {
  if (neighbor_pos[0] >= parameters.grid_size[0] || neighbor_pos[0] < 0 ||
      neighbor_pos[1] >= parameters.grid_size[1] || neighbor_pos[1] < 0 ||
      neighbor_pos[2] >= parameters.grid_size[2] || neighbor_pos[2] < 0) {
    return;
  }
  neighbor_combinations.push_back(neighbor_pos);
}

void grid::addNeighborCombinationVector(int const & i, int const & j, int const & k,
                                        std::vector<std::vector<int>> & neighbor_combinations) {
  addNeighborCombination(neighbor_combinations, std::vector<int>{i + 1, j + 1, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i + 1, j, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i, j + 1, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i, j, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i - 1, j, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i - 1, j - 1, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i - 1, j + 1, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i, j - 1, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i + 1, j - 1, k + 1});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i + 1, j, k});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i + 1, j + 1, k});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i, j + 1, k});
  addNeighborCombination(neighbor_combinations, std::vector<int>{i - 1, j + 1, k});
}

void grid::initializeNeighborCombinations() {
  int counter = 0;
  for (int i = 0; i < parameters.grid_size[0]; ++i) {
    for (int j = 0; j < parameters.grid_size[1]; ++j) {
      for (int k = 0; k < parameters.grid_size[2]; ++k) {
        std::vector<int> const pos = {i, j, k};
        grid_combinations.emplace_back(pos, counter);
        std::vector<std::vector<int>> neighbor_combinations;
        addNeighborCombinationVector(pos[0], pos[1], pos[2], neighbor_combinations);
        grid_neighbor_combinations.push_back(neighbor_combinations);
        counter++;
      }
    }
  }
}

void grid::repositionAndInitialize() {
  std::vector<std::vector<std::vector<block>>> new_part_grid;
  new_part_grid.resize(grid_size[0], std::vector<std::vector<block>>(grid_size[1], 
    std::vector<block>(grid_size[2])));

  for (unsigned int i = 0; i < part_dict.size(); ++i) {
    std::vector<int> pos;
    for (int j = 0; j < 3; ++j) {
      int const position = static_cast<int>(
          std::floor((part_dict[i].position[j] - min[j]) / block_size[j]));
      pos.push_back(std::max(0, std::min(position, grid_size[j] - 1)));
    }
    new_part_grid[pos[0]][pos[1]][pos[2]].particles.push_back(static_cast<int>(i));
  
    part_dict[i].density = 0.0;
    part_dict[i].acceleration = acceleration;
  }
  part_grid = new_part_grid;
}

double grid::geomNormSquared(std::vector<double> const & pos1, std::vector<double> const & pos2) {
  return std::pow(pos1[0] - pos2[0], 2) + std::pow(pos1[1] - pos2[1], 2) +
         std::pow(pos1[2] - pos2[2], 2);
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void grid::updateDensityBetweenParticles(particle & part1, particle & part2) {
  double const normSquared = geomNormSquared(part1.position, part2.position);
  if (normSquared < density_factors[0]) {
    double const densityIncrease  = std::pow(density_factors[0] - normSquared, 3);
    part1.density                += densityIncrease;
    part2.density                += densityIncrease;
  }
}

// factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }
void grid::updateAccelerationBetweenParticles(particle & part1, particle & part2) {
  double const normSquared = geomNormSquared(part1.position, part2.position);
  if (normSquared < acceleration_factors[0]) {
    double const dist                     = std::sqrt(std::max(normSquared, 1e-12));
    double const fluid_density_multiplier = 2.0;
    for (int i = 0; i < 3; ++i) {
      double delta_a =
          (part1.position[i] - part2.position[i]) * acceleration_factors[1] *
          (std::pow(smoothing_length - dist, 2) / dist) *
          (part1.density + part2.density - fluid_density_multiplier * constants::fluid_density);
      delta_a += (part2.velocity[i] - part1.velocity[i]) * acceleration_factors[2];
      delta_a /= (part1.density * part2.density);
      part1.acceleration[i] += delta_a;
      part2.acceleration[i] -= delta_a;
    }
  }
}

void grid::updateSameBlock(std::vector<int> const & pos, bool const updateType) {
  block part_block = part_grid[pos[0]][pos[1]][pos[2]];
  for (std::size_t i = 0; i < part_block.particles.size(); ++i) {
    for (std::size_t j = i + 1; j < part_block.particles.size(); ++j) {
      particle & part1 = part_dict[part_block.particles[i]];
      particle & part2 = part_dict[part_block.particles[j]];
      if (updateType) {
        updateDensityBetweenParticles(part1, part2);
      } else {
        updateAccelerationBetweenParticles(part1, part2);
      }
    }
  }
}

void grid::updateDifferentBlock(std::vector<int> const & pos1, std::vector<int> const & pos2,
                                bool const updateType) {
  if (pos1[0] >=grid_size[0] || pos1[0] < 0 || pos1[1] >= grid_size[1] ||
      pos1[1] < 0 || pos1[2] >= grid_size[2] || pos1[2] < 0 ||
      pos2[0] >= grid_size[0] || pos2[0] < 0 || pos2[1] >= grid_size[1] ||
      pos2[1] < 0 || pos2[2] >= grid_size[2] || pos2[2] < 0) {
    return;
  }

  block const block1 = part_grid[pos1[0]][pos1[1]][pos1[2]];
  block const block2 = part_grid[pos2[0]][pos2[1]][pos2[2]];
  for (int const & outer_index : block1.particles) {
    for (int const & inner_index : block2.particles) {
      if (updateType) {
        updateDensityBetweenParticles(part_dict[outer_index], part_dict[inner_index]);
      } else {
        updateAccelerationBetweenParticles(part_dict[outer_index], part_dict[inner_index]);
      }
    }
  }
}

// updateType = true: update density
// updateType = false: update acceleration
void grid::increaseVal(bool const updateType) {
  for (auto const & pos_pair : grid_combinations) {
    auto const & pos = pos_pair.first;
    updateSameBlock(pos, updateType);
    for (auto const & neighbor_pos : grid_neighbor_combinations[pos_pair.second]) {
      updateDifferentBlock(pos, neighbor_pos, updateType);
    }
  }
}

void grid::increaseSurroundingBlocks(int const & i, int const & j, int const & k, bool updateType) {
  updateSameBlock(std::vector<int>{i, j, k}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j + 1, k + 1},
                       updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j, k + 1}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j + 1, k + 1}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j, k + 1}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i - 1, j, k + 1}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i - 1, j - 1, k + 1},
                       updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i - 1, j + 1, k + 1},
                       updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j - 1, k + 1}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j - 1, k + 1},
                       updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j, k}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j + 1, k}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j + 1, k}, updateType);
  updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i - 1, j + 1, k}, updateType);
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void grid::densityTransform() {
  for (particle & part : part_dict) {
    part.density = (part.density + density_factors[1]) * density_factors[2];
  }
}

// if grid_positioning[index] == 0
void grid::updateAccelerationWithWallMin(particle & part, int const index) {
  double const newcoord       = part.position[index] + part.boundary[index] * constants::delt_t;
  double const delt           = constants::particle_size - (newcoord - min[index]);
  double const close_position = 1e-10;
  if (delt > close_position) {
    part.acceleration[index] +=
        constants::stiff_collisions * delt - constants::damping * part.velocity[index];
  }
}

// if grid_positioning[index] == grid_size[index] - 1
void grid::updateAccelerationWithWallMax(particle & part, int const index) {
  double const newcoord       = part.position[index] + part.boundary[index] * constants::delt_t;
  double const delt           = constants::particle_size - (max[index] - newcoord);
  double const close_position = 1e-10;
  if (delt > close_position) {
    part.acceleration[index] -=
        constants::stiff_collisions * delt + constants::damping * part.velocity[index];
  }
}

void grid::updateAccelerationWithWall(particle & part, std::vector<int> const & grid_position) {
  for (int i = 0; i < 3; i++) {
    if (grid_position[i] == 0) {
      updateAccelerationWithWallMin(part, i);
    } else if (grid_position[i] == grid_size[i] - 1) {
      updateAccelerationWithWallMax(part, i);
    }
  }
}

void grid::particlesMotion(particle & part) {
  double const magic_1 = 2.0;
  for (int i = 0; i < 3; ++i) {
    part.position[i] += part.boundary[i] * constants::delt_t +
                        part.acceleration[i] * constants::delt_t * constants::delt_t;
    part.velocity[i]  = part.boundary[i] + part.acceleration[i] * constants::delt_t / magic_1;
    part.boundary[i] += part.acceleration[i] * constants::delt_t;
  }
}

// if grid_positioning[index] == 0
void grid::collideWithWallMin(particle & part, int const index) {
  double const dist = part.position[index] - min[index];
  if (dist < 0) {
    part.position[index]  = min[index] - dist;
    part.velocity[index] *= -1.0;
    part.boundary[index] *= -1.0;
  }
}

// if grid_positioning[index] == grid_size[index] - 1
void grid::collideWithWallMax(particle & part, int const index) {
  double const dist = max[index] - part.position[index];
  if (dist < 0) {
    part.position[index]  = max[index] + dist;
    part.velocity[index] *= -1.0;
    part.boundary[index] *= -1.0;
  }
}

void grid::collideWithWall(particle & part, std::vector<int> const & grid_position) {
  for (int i = 0; i < 3; i++) {
    if (grid_position[i] == 0) {
      collideWithWallMin(part, i);
    } else if (grid_position[i] == grid_size[i] - 1) {
      collideWithWallMax(part, i);
    }
  }
}

void grid::processStep() {
  repositionAndInitialize();
  increaseVal(true);
  densityTransform();
  increaseVal(false);

  for (auto const & pos_pair : grid_combinations) {
    auto const & pos = pos_pair.first;
    for (int const & part_id : part_grid[pos[0]][pos[1]][pos[2]].particles) {
      particle & part = part_dict[part_id];
      updateAccelerationWithWall(part, pos);
      particlesMotion(part);
      collideWithWall(part, pos);
    }
  }
}