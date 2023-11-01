#include "grid.hpp"

grid::grid(std::istream & fileReader) {
  parameters.initialize(fileReader);
  int counter = 0;
  while (!fileReader.eof()) {
    particle part;
    part.id                     = counter;
    int const line_values       = 9;
    int const pos_conditional   = 3;
    int const bound_conditional = 6;
    for (int i = 0; i < line_values; ++i) {
      double const temp = read_float(fileReader);
      if (i < pos_conditional) {
        part.position.push_back(temp);
      } else if (i < bound_conditional) {
        part.boundary.push_back(temp);
      } else {
        part.velocity.push_back(temp);
      }
    }
    part_dict.push_back(part);
    counter++;
  }
  part_dict.pop_back();
}

void grid::repositionParticles() {
  std::vector<std::vector<std::vector<block>>> new_part_grid;
  std::vector<int> grid_size = parameters.grid_size;
  new_part_grid.resize(grid_size[0], std::vector<std::vector<block>>(
                                         grid_size[1], std::vector<block>(grid_size[2])));

  for (unsigned int i = 0; i < part_dict.size(); ++i) {
    std::vector<int> pos;
    for (int j = 0; j < 3; ++j) {
      int const position = static_cast<int>(
          std::floor((part_dict[i].position[j] - constants::min[j]) / parameters.block_size[j]));
      pos.push_back(std::max(0, std::min(position, parameters.grid_size[j] - 1)));
    }
    new_part_grid[pos[0]][pos[1]][pos[2]].particles.push_back(static_cast<int>(i));
  }
  part_grid = new_part_grid;
}

void grid::initializeDensityAndAcceleration() {
  for (particle & part : part_dict) {
    part.density      = 0.0;
    part.acceleration = constants::acceleration;
  }
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
  if (pos1[0] >= parameters.grid_size[0] || pos1[0] < 0 || pos1[1] >= parameters.grid_size[1] ||
      pos1[1] < 0 || pos1[2] >= parameters.grid_size[2] || pos1[2] < 0 ||
      pos2[0] >= parameters.grid_size[0] || pos2[0] < 0 || pos2[1] >= parameters.grid_size[1] ||
      pos2[1] < 0 || pos2[2] >= parameters.grid_size[2] || pos2[2] < 0) {
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

void grid::increaseSurroundingBlocks(const std::vector<int> &grid_position, bool updateType) {
    updateSameBlock(grid_position, updateType);

    // Precompute neighboring positions to avoid redundant calculations
    const std::vector<std::vector<int>> neighboring_positions = {
            {grid_position[0] + 1, grid_position[1], grid_position[2]},
            {grid_position[0], grid_position[1] + 1, grid_position[2]},
            // Add other neighboring positions as needed
    };

    for (const auto &pos : neighboring_positions) {
        updateDifferentBlock(grid_position, pos, updateType);
    }
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void grid::densityTransform() {
  for (particle & part : part_dict) {
    part.density = (part.density + parameters.density_factors[1]) * parameters.density_factors[2];
  }
}

// if grid_positioning[index] == 0
void grid::updateAccelerationWithWallMin(particle & part, int const index) {
  double const newcoord       = part.position[index] + part.boundary[index] * constants::delt_t;
  double const delt           = constants::particle_size - (newcoord - constants::min[index]);
  double const close_position = 1e-10;
  if (delt > close_position) {
    part.acceleration[index] +=
        constants::stiff_collisions * delt - constants::damping * part.velocity[index];
  }
}

// if grid_positioning[index] == grid_size[index] - 1
void grid::updateAccelerationWithWallMax(particle & part, int const index) {
  double const newcoord       = part.position[index] + part.boundary[index] * constants::delt_t;
  double const delt           = constants::particle_size - (constants::max[index] - newcoord);
  double const close_position = 1e-10;
  if (delt > close_position) {
    part.acceleration[index] -=
        constants::stiff_collisions * delt + constants::damping * part.velocity[index];
  }
}

void grid::updateAccelerationWithWall(particle & part, std::vector<int> const & grid_position) {
  if (grid_position[0] == 0) {
    updateAccelerationWithWallMin(part, 0);
  } else if (grid_position[0] == parameters.grid_size[0] - 1) {
    updateAccelerationWithWallMax(part, 0);
  }
  if (grid_position[1] == 0) {
    updateAccelerationWithWallMin(part, 1);
  } else if (grid_position[1] == parameters.grid_size[1] - 1) {
    updateAccelerationWithWallMax(part, 1);
  }
  if (grid_position[2] == 0) {
    updateAccelerationWithWallMin(part, 2);
  } else if (grid_position[2] == parameters.grid_size[2] - 1) {
    updateAccelerationWithWallMax(part, 2);
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
  double const dist = part.position[index] - constants::min[index];
  if (dist < 0) {
    part.position[index]  = constants::min[index] - dist;
    part.velocity[index] *= -1.0;
    part.boundary[index] *= -1.0;
  }
}

// if grid_positioning[index] == grid_size[index] - 1
void grid::collideWithWallMax(particle & part, int const index) {
  double const dist = constants::max[index] - part.position[index];
  if (dist < 0) {
    part.position[index]  = constants::max[index] + dist;
    part.velocity[index] *= -1.0;
    part.boundary[index] *= -1.0;
  }
}

void grid::collideWithWall(particle & part, std::vector<int> const & grid_position) {
  if (grid_position[0] == 0) {
    collideWithWallMin(part, 0);
  } else if (grid_position[0] == parameters.grid_size[0] - 1) {
    collideWithWallMax(part, 0);
  }
  if (grid_position[1] == 0) {
    collideWithWallMin(part, 1);
  } else if (grid_position[1] == parameters.grid_size[1] - 1) {
    collideWithWallMax(part, 1);
  }
  if (grid_position[2] == 0) {
    collideWithWallMin(part, 2);
  } else if (grid_position[2] == parameters.grid_size[2] - 1) {
    collideWithWallMax(part, 2);
  }
}

void grid::processStep() {
    repositionParticles();
    initializeDensityAndAcceleration();
    precomputeBlockValues(true);
    densityTransform();
    precomputeBlockValues(false);
    processParticles();
}

void grid::precomputeBlockValues(bool isFirstPass) {
    for (int i = 0; i < parameters.grid_size[0]; ++i) {
        for (int j = 0; j < parameters.grid_size[1]; ++j) {
            for (int k = 0; k < parameters.grid_size[2]; ++k) {
                const std::vector<int> grid_position = {i, j, k};
                increaseSurroundingBlocks(grid_position, isFirstPass);
            }
        }
    }
}

void grid::processParticles() {
    for (int i = 0; i < parameters.grid_size[0]; ++i) {
        for (int j = 0; j < parameters.grid_size[1]; ++j) {
            for (int k = 0; k < parameters.grid_size[2]; ++k) {
                const std::vector<int> grid_position = {i, j, k};
                increaseSurroundingBlocks(grid_position, false);
                const block &current_block = part_grid[i][j][k];
                for (const int part_id : current_block.particles) {
                    particle &part = part_dict[part_id];
                    updateAccelerationWithWall(part, grid_position);
                    particlesMotion(part);
                    collideWithWall(part, grid_position);
                }
            }
        }
    }
}
