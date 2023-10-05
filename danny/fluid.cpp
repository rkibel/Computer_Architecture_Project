#include "constants.hpp"
#include "utility.cpp"

#include <charconv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numbers>
#include <set>
#include <sstream>
#include <vector>

struct particle {
    int id;
    std::vector<double> position;
    std::vector<double> boundary;
    std::vector<double> velocity;
    std::vector<double> acceleration;
    std::vector<int> grid_positioning;
    double density;

    bool operator<(particle const & other) const { return id < other.id; }
};

double mass;
double smoothing_length;
double smoothing_length_squared;
std::vector<double> factorsDensity;
std::vector<double> factorsAcceleration;
std::vector<int> grid_size;
std::vector<double> block_size;

// think of this as a dictionary of the particles
std::vector<particle> particles;

// store only the particle ids in the grid
std::vector<std::vector<std::vector<std::set<int>>>> grid;

int parseInt(char * arg) {
  const std::string input_str = arg;
  int res;
  auto result = std::from_chars(input_str.data(), input_str.data() + input_str.size(), res);
  if (result.ec != std::errc()) {
    std::cerr << "Error: time steps must be numeric.\n";
    exit(1);
  }
  if (res < 0) {
    std::cerr << "Error: Invalid number of time steps.\n";
    exit(1);
  }
  return res;
}

void parseInput(char * inputFile) {
  std::ifstream fileReader;
  fileReader.open(inputFile, std::ios::binary);
  if (!fileReader) {
    std::cerr << "Error: Cannot open " << inputFile << " for reading";
    exit(1);
  }

  float ppm_read;
  int np;
  fileReader.read(reinterpret_cast<char *>(&ppm_read), sizeof(ppm_read));
  fileReader.read(reinterpret_cast<char *>(&np), sizeof(np));
  if (np <= 0) {
    std::cerr << "Error: Invalid number of particles: " << np << ".\n";
    exit(-5);
  }

  double const ppm         = ppm_read;
  mass                     = constants::fluid_density / ppm / ppm / ppm;
  smoothing_length         = constants::radius_mult / ppm;
  smoothing_length_squared = smoothing_length * smoothing_length;
  grid_size                = {
      static_cast<int>(std::floor((constants::max[0] - constants::min[0]) / smoothing_length)),
      static_cast<int>(std::floor((constants::max[1] - constants::min[1]) / smoothing_length)),
      static_cast<int>(std::floor((constants::max[2] - constants::min[2]) / smoothing_length))};
  block_size = {(constants::max[0] - constants::min[0]) / grid_size[0],
                (constants::max[1] - constants::min[1]) / grid_size[1],
                (constants::max[2] - constants::min[2]) / grid_size[2]};
  grid.resize(grid_size[0], std::vector<std::vector<std::set<int>>>(
                                grid_size[1], std::vector<std::set<int>>(grid_size[2])));

  int counter = 0;
  while (!fileReader.eof()) {
    particle p;
    p.id = counter;
    for (int i = 0; i < 9; ++i) {
      float temp;
      fileReader.read(reinterpret_cast<char *>(&temp), sizeof(float));
      if (i < 3) {
        p.position.push_back(temp);
      } else if (i < 6) {
        p.boundary.push_back(temp);
      } else {
        p.velocity.push_back(temp);
      }
    }
    particles.push_back(p);
    counter++;
  }
  particles.pop_back();

  if (counter - 1 != np) {
    std::cerr << "Error: Number of particles mismatch. Header: " << np << ", Found: " << counter - 1
              << ".\n";
    exit(-5);
  }

  std::cout << "Number of particles: " << np
            << "\n"
               "Particles per meter: "
            << ppm
            << "\n"
               "Smoothing length: "
            << smoothing_length
            << "\n"
               "Particle mass: "
            << mass
            << "\n"
               "Grid size: "
            << grid_size[0] << " x " << grid_size[1] << " x " << grid_size[2]
            << "\n"
               "Number of blocks: "
            << grid_size[0] * grid_size[1] * grid_size[2]
            << "\n"
               "Block size: "
            << block_size[0] << " x " << block_size[1] << " x " << block_size[2] << "\n";
}

void repositionParticles() {
  for (unsigned int i = 0; i < particles.size(); ++i) {
    particle & p            = particles[i];
    std::vector<int> orgpos = p.grid_positioning;
    std::vector<int> newpos;
    for (int j = 0; j < 3; ++j) {
      int position =
          static_cast<int>(std::floor((p.position[j] - constants::min[j]) / block_size[j]));
      newpos.push_back(std::max(0, std::min(position, grid_size[j] - 1)));
    }
    if (newpos != orgpos) {
      if (orgpos.size() == 3) { grid[orgpos[0]][orgpos[1]][orgpos[2]].erase(i); }
      grid[newpos[0]][newpos[1]][newpos[2]].insert(i);
      p.grid_positioning = newpos;
    }
  }
}

double geomNormSquared(std::vector<double> const & pos1, std::vector<double> const & pos2) {
  double result = 0;
  for (int i = 0; i < 3; ++i) {
    double diff  = pos1[i] - pos2[i];
    result      += diff * diff;
  }
  return result;
}

void initializeDensityAndAcceleration() {
  for (particle & part : particles) {
    part.density      = 0.0;
    part.acceleration = constants::acceleration;
  }
}

// factors = { h^2 }
void updateDensityBetweenParticles(int part1, int part2) {
  double normSquared = geomNormSquared(particles[part1].position, particles[part2].position);
  if (normSquared < smoothing_length_squared) {
    double densityIncrease    = std::pow(smoothing_length_squared - normSquared, 3);
    particles[part1].density += densityIncrease;
    particles[part2].density += densityIncrease;
  }
}

void densityTransform() {
  for (particle & part : particles) {
    part.density = (part.density + factorsDensity[0]) * factorsDensity[1];
  }
}

// factors = { 15/pi/h^6 * m , 45/pi/h^6 * mu * m }
void updateAccelerationBetweenParticles(int part1, int part2) {
  double normSquared = geomNormSquared(particles[part1].position, particles[part2].position);
  if (normSquared < smoothing_length_squared) {
    double dist = std::sqrt(std::max(normSquared, 1e-12));
    for (int i = 0; i < 3; ++i) {
      double delta_a =
          ((particles[part1].position[i] - particles[part2].position[i]) * factorsAcceleration[0] *
               (smoothing_length - dist) * (smoothing_length - dist) / dist *
               (particles[part1].density + particles[part2].density -
                2 * constants::fluid_density) +
           (particles[part2].velocity[i] - particles[part1].velocity[i]) * factorsAcceleration[1]) /
          particles[part1].density / particles[part2].density;

      std::cout << particles[part1].position[i] - particles[part2].position[i] << "\n";
      std::cout << factorsAcceleration[0] << "\n";
      std::cout << (smoothing_length - dist) * (smoothing_length - dist) / dist << "\n";
      std::cout << (particles[part1].density + particles[part2].density -
                    2 * constants::fluid_density)
                << "\n";
      std::cout << (particles[part2].velocity[i] - particles[part1].velocity[i]) << "\n";
      std::cout << particles[part1].density / particles[part2].density << "\n\n\n\n";
      std::cout << "DELTA A: " << delta_a << "\n";

      particles[part1].acceleration[i] += delta_a;
      particles[part2].acceleration[i] -= delta_a;
    }
  }
}

// updateType = true: update density
// updateType = false: update acceleration
void updateSameBlock(std::vector<int> pos, bool updateType) {
  std::set<int> particleSet = grid[pos[0]][pos[1]][pos[2]];
  for (auto it1 = particleSet.begin(); it1 != particleSet.end(); ++it1) {
    for (auto it2 = std::next(it1); it2 != particleSet.end(); ++it2) {
      if (updateType) {
        updateDensityBetweenParticles(*it1, *it2);
      } else {
        updateAccelerationBetweenParticles(*it1, *it2);
      }
    }
  }
}

// updateType = true: update density
// updateType = false: update acceleration
void updateDifferentBlock(std::vector<int> pos1, std::vector<int> pos2, bool updateType) {
  if (pos1[0] >= grid_size[0] || pos1[0] < 0 || pos1[1] >= grid_size[1] || pos1[1] < 0 ||
      pos1[2] >= grid_size[2] || pos1[2] < 0 || pos2[0] >= grid_size[0] || pos2[0] < 0 ||
      pos2[1] >= grid_size[1] || pos2[1] < 0 || pos2[2] >= grid_size[2] || pos2[2] < 0) {
    return;
  }

  std::set<int> set1 = grid[pos1[0]][pos1[1]][pos1[2]];
  std::set<int> set2 = grid[pos2[0]][pos2[1]][pos2[2]];
  for (auto it1 = set1.begin(); it1 != set1.end(); ++it1) {
    for (auto it2 = set2.begin(); it2 != set2.end(); ++it2) {
      if (updateType) {
        updateDensityBetweenParticles(*it1, *it2);
      } else {
        updateAccelerationBetweenParticles(*it1, *it2);
      }
    }
  }
}

// updateType = true: update density
// updateType = false: update acceleration
void densityOrAccelerationUpdate(bool updateType) {
  for (int i = 0; i < grid_size[0]; ++i) {
    for (int j = 0; j < grid_size[1]; ++j) {
      for (int k = 0; k < grid_size[2]; ++k) {
        updateSameBlock(std::vector<int>{i, j, k}, updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j, k}, updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j + 1, k},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j + 1, k}, updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i - 1, j + 1, k},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j, k + 1}, updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j, k + 1},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j + 1, k + 1},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j + 1, k + 1},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i - 1, j + 1, k + 1},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i - 1, j, k + 1},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i - 1, j - 1, k + 1},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j - 1, k + 1},
                             updateType);
        updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i + 1, j - 1, k + 1},
                             updateType);
      }
    }
  }
}

void updateAccelerationWithWall(particle & part, int index, bool boundaryType) {
  if (part.grid_positioning[index] == 0 || part.grid_positioning[index] == grid_size[index] - 1) {
    double newcoord = part.position[index] + part.boundary[index] * constants::delt_t;
    double delt     = constants::particle_size - (boundaryType ? (newcoord - constants::min[index])
                                                               : (constants::max[index] - newcoord));
    if (delt > 1e-10) {
      double factor =
          constants::stiff_collisions * delt - constants::damping * part.velocity[index];
      if (boundaryType) {
        part.acceleration[index] += factor;
      } else {
        part.acceleration[index] -= factor;
      }
    }
  }
}

void particlesMotion(particle & part, int index) {
  part.position[index] += part.boundary[index] * constants::delt_t +
                          part.acceleration[index] * constants::delt_t * constants::delt_t;
  part.velocity[index]  = part.boundary[index] + part.acceleration[index] * constants::delt_t / 2.0;
  part.boundary[index] += part.acceleration[index] * constants::delt_t;
}

void collideWithWall(particle & part, int index, bool boundaryType) {
  if (part.grid_positioning[index] == 0) {
    double dist = boundaryType ? (part.position[index] - constants::min[index])
                               : (constants::max[index] - part.position[index]);
    if (dist < 0) {
      part.position[index] =
          boundaryType ? (constants::min[index] - dist) : (constants::max[index] + dist);
      part.velocity[index] *= -1.0;
      part.boundary[index] *= -1.0;
    }
  }
}

void processStep() {
  repositionParticles();
  initializeDensityAndAcceleration();
  densityOrAccelerationUpdate(true);  // update density
  std::cout << particles[0].id << " " << particles[0].position[0] << " " << particles[0].position[1]
            << " " << particles[0].position[2] << "\n";
  std::cout << particles[0].id << " " << particles[0].density << "\n";
  std::cout << "Acceleration: " << particles[0].acceleration[0] << " "
            << particles[0].acceleration[1] << " " << particles[0].acceleration[1] << "\n";
  densityTransform();
  std::cout << particles[0].id << " " << particles[0].position[0] << " " << particles[0].position[1]
            << " " << particles[0].position[2] << "\n";
  std::cout << particles[0].id << " " << particles[0].density << "\n";
  std::cout << "Acceleration: " << particles[0].acceleration[0] << " "
            << particles[0].acceleration[1] << " " << particles[0].acceleration[1] << "\n";
  densityOrAccelerationUpdate(false);  // update acceleration
  std::cout << particles[0].id << " " << particles[0].position[0] << " " << particles[0].position[1]
            << " " << particles[0].position[2] << "\n";
  std::cout << particles[0].id << " " << particles[0].density << "\n";
  std::cout << "Acceleration: " << particles[0].acceleration[0] << " "
            << particles[0].acceleration[1] << " " << particles[0].acceleration[1] << "\n";

  for (particle & part : particles) {
    for (int i = 0; i < 3; ++i) {
      bool isBoundaryParticle =
          (part.grid_positioning[i] == 0 || part.grid_positioning[i] == grid_size[i] - 1);
      // boundaryType = True means 0 boundary, False means size-1
      bool boundaryType = part.grid_positioning[i] == 0;
      if (isBoundaryParticle) { updateAccelerationWithWall(part, i, boundaryType); }
      particlesMotion(part, i);
      if (isBoundaryParticle) { collideWithWall(part, i, boundaryType); }
    }
  }
  std::cout << particles[0].id << " " << particles[0].position[0] << " " << particles[0].position[1]
            << " " << particles[0].position[2] << "\n";
  std::cout << particles[0].id << " " << particles[0].density << "\n";
  std::cout << "Acceleration: " << particles[0].acceleration[0] << " "
            << particles[0].acceleration[1] << " " << particles[0].acceleration[1] << "\n";
}

void testOutput(char * outputFile) {
  return;
}

/*void writeFile(std::string outputFile, float ppm, int np, std::vector<particle> particles) {
    std::ofstream file;
    file.open(outputFile, std::ios::binary);
    file.write(reinterpret_cast<char*>(&ppm), sizeof(ppm));
    file.write(reinterpret_cast<char*>(&np), sizeof(np));
    for (particle p: particles) {
        float px = p.px;
        float py = p.py;
        float pz = p.pz;
        float hvx = p.hvx;
        float hvy = p.hvy;
        float hvz = p.hvz;
        float vx = p.vx;
        float vy = p.vy;
        float vz = p.vz;
        file.write(reinterpret_cast<char*>(&px), sizeof(float));
        file.write(reinterpret_cast<char*>(&py), sizeof(float));
        file.write(reinterpret_cast<char*>(&pz), sizeof(float));
        file.write(reinterpret_cast<char*>(&hvx), sizeof(float));
        file.write(reinterpret_cast<char*>(&hvy), sizeof(float));
        file.write(reinterpret_cast<char*>(&hvz), sizeof(float));
        file.write(reinterpret_cast<char*>(&vx), sizeof(float));
        file.write(reinterpret_cast<char*>(&vy), sizeof(float));
        file.write(reinterpret_cast<char*>(&vz), sizeof(float));
    }
}*/

/*
    run with:
    g++ -o fluid fluid.cpp -std=c++20
    ./fluid **timestep** **inputfile** **outputfile**
*/
int main(int argc, char * argv[]) {
  if (argc != 4) {
    std::cerr << "Error: Invalid number of arguments: " << argc - 1 << ".\n";
    return 1;
  }

  int nts = parseInt(argv[1]);
  parseInput(argv[2]);
  testOutput(argv[3]);

  // Creating factors to use in simulation steps
  factorsDensity = {std::pow(smoothing_length, 6),
                    315.0 * mass / 64.0 / std::numbers::pi / std::pow(smoothing_length, 9)};
  double factor  = 15.0 / std::numbers::pi / std::pow(smoothing_length, 6) * mass;

  factorsAcceleration = {factor, factor * 3.0 * constants::viscosity};

  for (unsigned int i = 0; i < 1; ++i) {
    std::cout << "step " << i << "\n";
    processStep();
  }

  for (particle p : particles) {
    std::cout << p.id << " " << p.position[0] << " " << p.position[1] << " " << p.position[2]
              << "\n";
    break;
    std::cout << p.grid_positioning[0] << " " << p.grid_positioning[1] << " "
              << p.grid_positioning[2] << "\n";
  }

  return 0;
}