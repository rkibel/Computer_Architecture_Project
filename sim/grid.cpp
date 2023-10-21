#include "grid.hpp"

//#include "utility.cpp"

template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char * as_writable_buffer(T & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char *>(&value);
}

template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
T read_binary_value(std::istream & is) {
  T value{};
  is.read(as_writable_buffer(value), sizeof(value));
  return value;
}

grid::grid(std::istream& fileReader) {
    parameters.initialize(fileReader);
    int counter = 0;
    while (!fileReader.eof()) {
        particle p;
        p.id = counter;
        for (int i = 0; i < 9; ++i) {
            float temp = read_binary_value<float>(fileReader);
            if (i < 3) p.position.push_back(temp);
            else if (i < 6) p.boundary.push_back(temp);
            else p.velocity.push_back(temp);
        }
        part_dict.push_back(p);
        counter++;
    }
    part_dict.pop_back();
}

void grid::repositionParticles() {
    std::vector<std::vector<std::vector<block>>> new_part_grid;
    std::vector<int> grid_size = parameters.grid_size;
    new_part_grid.resize(grid_size[0], std::vector<std::vector<block>>(grid_size[1], std::vector<block>(grid_size[2])));
    
    for (int i = 0; i < part_dict.size(); ++i) {
        std::vector<int> pos;
        for (int j = 0; j < 3; ++j) {
            int position = static_cast<int>(std::floor((part_dict[i].position[j] - constants::min[j]) / parameters.block_size[j]));
            pos.push_back(std::max(0, std::min(position, parameters.grid_size[j]-1))); 
        }
        new_part_grid[pos[0]][pos[1]][pos[2]].push_back(i);
    }
    part_grid = new_part_grid;
}

void grid::initializeDensityAndAcceleration() {
    for (particle& part: part_dict) {
        part.density = 0.0;
        part.acceleration = constants::acceleration;
    }
}

double grid::geomNormSquared(const std::vector<double> pos1, const std::vector<double> pos2) {
    return std::pow(pos1[0] - pos2[0], 2) + 
           std::pow(pos1[1] - pos2[1], 2) + 
           std::pow(pos1[2] - pos2[2], 2);
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void grid::updateDensityBetweenParticles(particle& part1, particle& part2) {
    double normSquared = geomNormSquared(part1.position, part2.position);
    if (normSquared < parameters.density_factors[0]) {
        double densityIncrease = std::pow(parameters.density_factors[0] - normSquared, 3);
        part1.density += densityIncrease;
        part2.density += densityIncrease;
    }
}

// factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }
void grid::updateAccelerationBetweenParticles(particle& part1, particle& part2) {
    double normSquared = geomNormSquared(part1.position, part2.position);
    if (normSquared < parameters.acceleration_factors[0]) {
        double dist = std::sqrt(std::max(normSquared, 1e-12));
        for (int i = 0; i < 3; ++i) {
            double delta_a =
                (part1.position[i] - part2.position[i]) *
                parameters.acceleration_factors[1] * \
                (std::pow(parameters.smoothing_length - dist, 2) / dist) * \
                (part1.density + part2.density - 2.0 * constants::fluid_density);
            delta_a += (part2.velocity[i] - part1.velocity[i]) * parameters.acceleration_factors[2];
            delta_a /= (part1.density * part2.density);
            part1.acceleration[i] += delta_a;
            part2.acceleration[i] -= delta_a;
        }
    }
}

void grid::updateSameBlock(std::vector<int> pos, bool updateType) {
    block part_block = part_grid[pos[0]][pos[1]][pos[2]];
    for (int i = 0; i < part_block.size(); ++i) {
        for (int j = i+1; j < part_block.size(); ++j) {
            particle& part1 = part_dict[part_block.getBlockId(i)];
            particle& part2 = part_dict[part_block.getBlockId(j)];
            if (updateType) updateDensityBetweenParticles(part1, part2);
            else updateAccelerationBetweenParticles(part1, part2);
        }
    }
}

void grid::updateDifferentBlock(std::vector<int> pos1, std::vector<int> pos2, bool updateType) {
    if (pos1[0] >= parameters.grid_size[0] || pos1[0] < 0 ||
        pos1[1] >= parameters.grid_size[1] || pos1[1] < 0 ||
        pos1[2] >= parameters.grid_size[2] || pos1[2] < 0 ||
        pos2[0] >= parameters.grid_size[0] || pos2[0] < 0 ||
        pos2[1] >= parameters.grid_size[1] || pos2[1] < 0 ||
        pos2[2] >= parameters.grid_size[2] || pos2[2] < 0) return; 
    
    block block1 = part_grid[pos1[0]][pos1[1]][pos1[2]];
    block block2 = part_grid[pos2[0]][pos2[1]][pos2[2]];
    for (int& i: block1.particles) {
        for (int& j: block2.particles) {
            if (updateType) updateDensityBetweenParticles(part_dict[i], part_dict[j]);
            else updateAccelerationBetweenParticles(part_dict[i], part_dict[j]);
        }
    }
}

// updateType = true: update density
// updateType = false: update acceleration
void grid::increaseVal(bool updateType) {
    for (int i = 0; i < parameters.grid_size[0]; ++i) {
        for (int j = 0; j < parameters.grid_size[1]; ++j) {
            for (int k = 0; k < parameters.grid_size[2]; ++k) {
                updateSameBlock(std::vector<int>{i, j, k}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j+1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j+1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i-1, j, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i-1, j-1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i-1, j+1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j-1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j-1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j, k}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j+1, k}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j+1, k}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i-1, j+1, k}, updateType);
            }
        }
    }
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void grid::densityTransform() {
    for (particle& part: part_dict) {
        part.density = (part.density + parameters.density_factors[1]) * parameters.density_factors[2];
    }
}

// if grid_positioning[index] == 0
void grid::updateAccelerationWithWallMin(particle& part, int index) {
    double newcoord = part.position[index] + part.boundary[index] * constants::delt_t;
    double delt = constants::particle_size - (newcoord - constants::min[index]);
    if (delt > 1e-10) 
        part.acceleration[index] += constants::stiff_collisions * delt - constants::damping * part.velocity[index];
}

// if grid_positioning[index] == grid_size[index] - 1
void grid::updateAccelerationWithWallMax(particle& part, int index) {
    double newcoord = part.position[index] + part.boundary[index] * constants::delt_t;
    double delt = constants::particle_size - (constants::max[index] - newcoord);
    if (delt > 1e-10)
        part.acceleration[index] -= constants::stiff_collisions * delt + constants::damping * part.velocity[index];;
}

void grid::updateAccelerationWithWall(particle& part, int x, int y, int z) {
    if (x == 0) updateAccelerationWithWallMin(part, 0);
    else if (x == parameters.grid_size[0] - 1) updateAccelerationWithWallMax(part, 0);
    if (y == 0) updateAccelerationWithWallMin(part, 1);
    else if (y == parameters.grid_size[1] - 1) updateAccelerationWithWallMax(part, 1);
    if (z == 0) updateAccelerationWithWallMin(part, 2);
    else if (z == parameters.grid_size[2] - 1) updateAccelerationWithWallMax(part, 2);
}

void grid::particlesMotion(particle& part) {
    for (int i = 0; i < 3; ++i) {
        part.position[i] += part.boundary[i] * constants::delt_t + part.acceleration[i] * constants::delt_t * constants::delt_t;
        part.velocity[i] = part.boundary[i] + part.acceleration[i] * constants::delt_t / 2.0;
        part.boundary[i] += part.acceleration[i] * constants::delt_t;
    }
}

// if grid_positioning[index] == 0
void grid::collideWithWallMin(particle& part, int index) {
    double dist = part.position[index] - constants::min[index];
    if (dist < 0) {
        part.position[index] = constants::min[index] - dist;
        part.velocity[index] *= -1.0;
        part.boundary[index] *= -1.0;
    }
}

// if grid_positioning[index] == grid_size[index] - 1
void grid::collideWithWallMax(particle& part, int index) {
    double dist = constants::max[index] - part.position[index];
    if (dist < 0) {
        part.position[index] = constants::max[index] + dist;
        part.velocity[index] *= -1.0;
        part.boundary[index] *= -1.0;
    }
}

void grid::collideWithWall(particle& part, int x, int y, int z) {
    if (x == 0) collideWithWallMin(part, 0);
    else if (x == parameters.grid_size[0] - 1) collideWithWallMax(part, 0);
    if (y == 0) collideWithWallMin(part, 1);
    else if (y == parameters.grid_size[1] - 1) collideWithWallMax(part, 1);
    if (z == 0) collideWithWallMin(part, 2);
    else if (z == parameters.grid_size[2] - 1) collideWithWallMax(part, 2);
}

void grid::processStep() {
    repositionParticles();
    initializeDensityAndAcceleration();
    increaseVal(true);
    densityTransform();
    increaseVal(false);
    for (int i = 0; i < parameters.grid_size[0]; ++i) {
        for (int j = 0; j < parameters.grid_size[1]; ++j) {
            for (int k = 0; k < parameters.grid_size[2]; ++k) {
                for (int& part_id: part_grid[i][j][k].particles) {
                    particle& part = part_dict[part_id];
                    updateAccelerationWithWall(part, i, j, k);
                    particlesMotion(part);
                    collideWithWall(part, i, j, k);
                }
            }
        }
    }
}