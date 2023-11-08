#include "grid.hpp"

grid::grid(std::istream & fileReader) {
  parameters.initialize(fileReader);
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
  initializeNeighborCombinations();
}

bool grid::isOutsideGrid(int i, int j, int k) const {
  return (i >= parameters.grid_size[0] || i < 0 || j >= parameters.grid_size[1] || j < 0 ||
          k >= parameters.grid_size[2] || k < 0);
}

std::vector<std::vector<int>> grid::getNeighbors(int i, int j, int k) const {
  std::vector<std::vector<int>> neighbors;
  neighbors.push_back(std::vector<int>{i, j, k});
  if (!isOutsideGrid(i + 1, j + 1, k + 1)) {
    neighbors.push_back(std::vector<int>{i + 1, j + 1, k + 1});
  }
  if (!isOutsideGrid(i + 1, j, k + 1)) { neighbors.push_back(std::vector<int>{i + 1, j, k + 1}); }
  if (!isOutsideGrid(i, j + 1, k + 1)) { neighbors.push_back(std::vector<int>{i, j + 1, k + 1}); }
  if (!isOutsideGrid(i, j, k + 1)) { neighbors.push_back(std::vector<int>{i, j, k + 1}); }
  if (!isOutsideGrid(i - 1, j, k + 1)) { neighbors.push_back(std::vector<int>{i - 1, j, k + 1}); }
  if (!isOutsideGrid(i - 1, j - 1, k + 1)) {
    neighbors.push_back(std::vector<int>{i - 1, j - 1, k + 1});
  }
  if (!isOutsideGrid(i - 1, j + 1, k + 1)) {
    neighbors.push_back(std::vector<int>{i - 1, j + 1, k + 1});
  }
  if (!isOutsideGrid(i, j - 1, k + 1)) { neighbors.push_back(std::vector<int>{i, j - 1, k + 1}); }
  if (!isOutsideGrid(i + 1, j - 1, k + 1)) {
    neighbors.push_back(std::vector<int>{i + 1, j - 1, k + 1});
  }
  if (!isOutsideGrid(i + 1, j, k)) { neighbors.push_back(std::vector<int>{i + 1, j, k}); }
  if (!isOutsideGrid(i + 1, j + 1, k)) { neighbors.push_back(std::vector<int>{i + 1, j + 1, k}); }
  if (!isOutsideGrid(i, j + 1, k)) { neighbors.push_back(std::vector<int>{i, j + 1, k}); }
  if (!isOutsideGrid(i - 1, j + 1, k)) { neighbors.push_back(std::vector<int>{i - 1, j + 1, k}); }
  return neighbors;
}

void grid::initializeNeighborCombinations() {
  for (int i = 0; i < parameters.grid_size[0]; ++i) {
    for (int j = 0; j < parameters.grid_size[1]; ++j) {
      for (int k = 0; k < parameters.grid_size[2]; ++k) {
        grid_neighbor_combinations.push_back(getNeighbors(i, j, k));
      }
    }
  }
}

void grid::repositionAndInitialize() {
  std::vector<std::vector<std::vector<block>>> new_part_grid;
  std::vector<int> grid_size = parameters.grid_size;
  new_part_grid.resize(grid_size[0], std::vector<std::vector<block>>(
                                         grid_size[1], std::vector<block>(grid_size[2])));

  for (unsigned int i = 0; i < part_dict.size(); ++i) {
    std::vector<int> pos;
    for (int j = 0; j < 3; ++j) {
      int const position = static_cast<int>(
          std::floor((part_dict[i].position[j] - parameters.min[j]) / parameters.block_size[j]));
      pos.push_back(std::max(0, std::min(position, parameters.grid_size[j] - 1)));
    }
    new_part_grid[pos[0]][pos[1]][pos[2]].particles.push_back(static_cast<int>(i));
    part_dict[i].grid_pos     = pos;
    part_dict[i].density      = 0.0;
    part_dict[i].acceleration = parameters.acceleration;
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
  if (normSquared < parameters.density_factors[0]) {
    double const densityIncrease  = std::pow(parameters.density_factors[0] - normSquared, 3);
    part1.density                += densityIncrease;
    part2.density                += densityIncrease;
  }
}

// factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }
void grid::updateAccelerationBetweenParticles(particle & part1, particle & part2) {
  double const normSquared = geomNormSquared(part1.position, part2.position);
  if (normSquared < parameters.acceleration_factors[0]) {
    double const dist                     = std::sqrt(std::max(normSquared, 1e-12));
    double const fluid_density_multiplier = 2.0;
    for (int i = 0; i < 3; ++i) {
      double delta_a =
          (part1.position[i] - part2.position[i]) * parameters.acceleration_factors[1] *
          (std::pow(parameters.smoothing_length - dist, 2) / dist) *
          (part1.density + part2.density - fluid_density_multiplier * constants::fluid_density);
      delta_a += (part2.velocity[i] - part1.velocity[i]) * parameters.acceleration_factors[2];
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
  for (auto const & surround : grid_neighbor_combinations) {
    updateSameBlock(surround[0], updateType);
    for (size_t i = 1; i < surround.size(); ++i) {
      updateDifferentBlock(surround[0], surround[i], updateType);
    }
  }
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void grid::densityTransform() {
  for (particle & part : part_dict) {
    part.density = (part.density + parameters.density_factors[1]) * parameters.density_factors[2];
  }
}

// if grid_positioning[index] == 0
void grid::updateAccWithWallMin(particle & part, int const index) {
  double const newcoord       = part.position[index] + part.boundary[index] * constants::delt_t;
  double const delt           = constants::particle_size - (newcoord - parameters.min[index]);
  double const close_position = 1e-10;
  if (delt > close_position) {
    part.acceleration[index] +=
        constants::stiff_collisions * delt - constants::damping * part.velocity[index];
  }
}

// if grid_positioning[index] == grid_size[index] - 1
void grid::updateAccWithWallMax(particle & part, int const index) {
  double const newcoord       = part.position[index] + part.boundary[index] * constants::delt_t;
  double const delt           = constants::particle_size - (parameters.max[index] - newcoord);
  double const close_position = 1e-10;
  if (delt > close_position) {
    part.acceleration[index] -=
        constants::stiff_collisions * delt + constants::damping * part.velocity[index];
  }
}

void grid::updateAccWithWall(particle & part) {
  for (int i = 0; i < 3; i++) {
    if (part.grid_pos[i] == 0) {
      updateAccWithWallMin(part, i);
    } else if (part.grid_pos[i] == parameters.grid_size[i] - 1) {
      updateAccWithWallMax(part, i);
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
  double const dist = part.position[index] - parameters.min[index];
  if (dist < 0) {
    part.position[index]  = parameters.min[index] - dist;
    part.velocity[index] *= -1.0;
    part.boundary[index] *= -1.0;
  }
}

// if grid_positioning[index] == grid_size[index] - 1
void grid::collideWithWallMax(particle & part, int const index) {
  double const dist = parameters.max[index] - part.position[index];
  if (dist < 0) {
    part.position[index]  = parameters.max[index] + dist;
    part.velocity[index] *= -1.0;
    part.boundary[index] *= -1.0;
  }
}

void grid::collideWithWall(particle & part) {
  for (int i = 0; i < 3; i++) {
    if (part.grid_pos[i] == 0) {
      collideWithWallMin(part, i);
    } else if (part.grid_pos[i] == parameters.grid_size[i] - 1) {
      collideWithWallMax(part, i);
    }
  }
}

void grid::processStep() {
  repositionAndInitialize();
  increaseVal(true);
  densityTransform();
  increaseVal(false);
  for (particle & part : part_dict) {
    updateAccWithWall(part);
    particlesMotion(part);
    collideWithWall(part);
  }
}