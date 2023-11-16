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
  return (i >= parameters.grid_size[0] || i < 0 ||
          j >= parameters.grid_size[1] || j < 0 ||
          k >= parameters.grid_size[2] || k < 0);
}

std::vector<std::vector<int>> grid::getNeighbors(int i, int j, int k) const {
  std::vector<std::vector<int>> neighbors;
  neighbors.push_back(std::vector<int>{i, j, k});
  if (!isOutsideGrid(i+1, j+1, k+1)) { neighbors.push_back(std::vector<int>{i+1, j+1, k+1}); }
  if (!isOutsideGrid(i+1, j, k+1)) { neighbors.push_back(std::vector<int>{i+1, j, k+1}); }
  if (!isOutsideGrid(i, j+1, k+1)) { neighbors.push_back(std::vector<int>{i, j+1, k+1}); }
  if (!isOutsideGrid(i, j, k+1)) { neighbors.push_back(std::vector<int>{i, j, k+1}); }
  if (!isOutsideGrid(i-1, j, k+1)) { neighbors.push_back(std::vector<int>{i-1, j, k+1}); }
  if (!isOutsideGrid(i-1, j-1, k+1)) { neighbors.push_back(std::vector<int>{i-1, j-1, k+1}); }
  if (!isOutsideGrid(i-1, j+1, k+1)) { neighbors.push_back(std::vector<int>{i-1, j+1, k+1}); }
  if (!isOutsideGrid(i, j-1, k+1)) { neighbors.push_back(std::vector<int>{i, j-1, k+1}); }
  if (!isOutsideGrid(i+1, j-1, k+1)) { neighbors.push_back(std::vector<int>{i+1, j-1, k+1}); }
  if (!isOutsideGrid(i+1, j, k)) { neighbors.push_back(std::vector<int>{i+1, j, k}); }
  if (!isOutsideGrid(i+1, j+1, k)) { neighbors.push_back(std::vector<int>{i+1, j+1, k}); }
  if (!isOutsideGrid(i, j+1, k)) { neighbors.push_back(std::vector<int>{i, j+1, k}); }
  if (!isOutsideGrid(i-1, j+1, k)) { neighbors.push_back(std::vector<int>{i-1, j+1, k}); }
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

  for (size_t i = 0; i < part_dict.size(); ++i) {
    std::vector<int> pos;
    for (int j = 0; j < 3; ++j) {
      int const position = static_cast<int>(
          std::floor((part_dict[i].position[j] - parameters.min[j]) / parameters.block_size[j]));
      pos.push_back(std::max(0, std::min(position, parameters.grid_size[j] - 1)));
    }
    new_part_grid[pos[0]][pos[1]][pos[2]].particles.push_back(static_cast<int>(i));
    part_dict[i].grid_pos     = pos;
    part_dict[i].density      = 0.0;
    part_dict[i].acceleration = {0, constants::grav, 0};
  }
  part_grid = new_part_grid;
}

double grid::normSquared(std::vector<double> const & pos1, std::vector<double> const & pos2) {
  return std::pow(pos1[0] - pos2[0], 2) + std::pow(pos1[1] - pos2[1], 2) +
         std::pow(pos1[2] - pos2[2], 2);
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void grid::updateDensity(particle & part1, particle & part2) {
  double const norm_squared = normSquared(part1.position, part2.position);
  if (norm_squared < parameters.density_factors[0]) {
    double const densityIncrease  = std::pow(parameters.density_factors[0] - norm_squared, 3);
    part1.density                += densityIncrease;
    part2.density                += densityIncrease;
  }
}

// factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }
void grid::updateAcceleration(particle & part1, particle & part2) {
  double const norm_squared = normSquared(part1.position, part2.position);
  if (norm_squared < parameters.acceleration_factors[0]) {
    double const dist                     = std::sqrt(std::max(norm_squared, 1e-12));
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

// type = true: update density
// type = false: update acceleration
void grid::updateBlock(std::vector<int> const & pos1, std::vector<int> const & pos2,
                      bool const type) {
  const block & block1 = part_grid[pos1[0]][pos1[1]][pos1[2]];
  const block & block2 = part_grid[pos2[0]][pos2[1]][pos2[2]];
  for (std::size_t i = 0; i < block1.particles.size(); ++i) {
    const std::size_t loop_start = (pos1 == pos2) ? i+1 : 0;
    for (std::size_t j = loop_start; j < block2.particles.size(); ++j) {
      particle & part1 = part_dict[block1.particles[i]];
      particle & part2 = part_dict[block2.particles[j]];
      type ? updateDensity(part1, part2) : updateAcceleration(part1, part2);
    }
  }
}

void grid::updateAccWithWall(particle & part) {
  for (int i = 0; i < 3; i++) {
    if (part.grid_pos[i] == 0) {
      double const newcoord       = part.position[i] + part.boundary[i] * constants::delt_t;
      double const delt           = constants::particle_size - (newcoord - parameters.min[i]);
      double const close_position = 1e-10;
      if (delt > close_position) {
        part.acceleration[i] +=
            constants::stiff_collisions * delt - constants::damping * part.velocity[i];
      }
    } 
    else if (part.grid_pos[i] == parameters.grid_size[i] - 1) {
      double const newcoord       = part.position[i] + part.boundary[i] * constants::delt_t;
      double const delt           = constants::particle_size - (parameters.max[i] - newcoord);
      double const close_position = 1e-10;
      if (delt > close_position) {
        part.acceleration[i] -=
            constants::stiff_collisions * delt + constants::damping * part.velocity[i];
      }
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

void grid::collideWithWall(particle & part) {
  for (int i = 0; i < 3; i++) {
    if (part.grid_pos[i] == 0) {
    double const dist = part.position[i] - parameters.min[i];
      if (dist < 0) {
        part.position[i]  = parameters.min[i] - dist;
        part.velocity[i] *= -1.0;
        part.boundary[i] *= -1.0;
      }
    }
    else if (part.grid_pos[i] == parameters.grid_size[i] - 1) {
      double const dist = parameters.max[i] - part.position[i];
      if (dist < 0) {
        part.position[i]  = parameters.max[i] + dist;
        part.velocity[i] *= -1.0;
        part.boundary[i] *= -1.0;
      }
    }
  }
}

void grid::processStep() {
  repositionAndInitialize();
  for (auto const & surround : grid_neighbor_combinations) {
    for (size_t i = 0; i < surround.size(); ++i) { 
      updateBlock(surround[0], surround[i], true);
    }
  }
  for (particle & part : part_dict) {
    part.density = (part.density + parameters.density_factors[1]) * parameters.density_factors[2];
  }
  for (auto const & surround : grid_neighbor_combinations) {
    for (size_t i = 0; i < surround.size(); ++i) { 
      updateBlock(surround[0], surround[i], false);
    }
  }
  for (particle& part: part_dict) {
    updateAccWithWall(part);
    particlesMotion(part);
    collideWithWall(part);
  }
}