#include "constants.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numbers>
#include <set>
#include <sstream>
#include <typeinfo>
#include <vector>

struct particle {
    int const id;
    // will resize these three vectors when reading
    std::vector<double> position_vector;
    std::vector<double> boundary_vector;  // represents hv, which means boundary velocity
    std::vector<double> velocity_vector;
    std::vector<double> acceleration_vector;
    double density;

    // comparing id
    bool operator<(particle const & other) const { return id < other.id; }
};

struct block_parameters {
    double mass;
    double smoothing_length_h;
    std::vector<int> grid_size;
};

// Putting outside struct since we don't need to access it much
std::vector<double> block_size;

// Putting following two outside of the block_parameters struct so entire struct doesn't need to
// load each time
std::vector<particle> particles;  // think of this as a dictionary of the particles, ie in
                                  // position 2 is info about particle with id=2
std::vector<std::vector<std::vector<std::set<int>>>>
    grid;  // store only the particle ids in the grid

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

void parseInput(char * inputFile, block_parameters & blocks) {
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

  // Initializing parameters and simulation_block
  double const ppm          = static_cast<double>(ppm_read);
  double mass               = constants::rho / ppm / ppm / ppm;
  double smoothing_length_h = constants::r / ppm;
  std::vector<int> grid_size{
      static_cast<int>(std::floor((constants::max[0] - constants::min[0]) / smoothing_length_h)),
      static_cast<int>(std::floor((constants::max[1] - constants::min[1]) / smoothing_length_h)),
      static_cast<int>(std::floor((constants::max[2] - constants::min[2]) / smoothing_length_h))};

  std::vector<double> block_size{(constants::max[0] - constants::min[0]) / grid_size[0],
                                 (constants::max[1] - constants::min[0]) / grid_size[1],
                                 (constants::max[2] - constants::min[0]) / grid_size[2]};

  grid.resize(grid_size[0], std::vector<std::vector<std::set<int>>>(
                                grid_size[1], std::vector<std::set<int>>(grid_size[2])));

  std::cout << "Testing 7 10 7 in ParseInput\n";
  if (grid[7][10][7].empty()) { std::cout << "YO empty\n"; }

  // std::cout << "Grid size check\n"
  //           << grid.size() << "X" << grid[0].size() << "X" << grid[0][0].size() << "\n";

  blocks = block_parameters{mass, smoothing_length_h, grid_size};

  // Reading particles
  int counter = 0;

  while (!fileReader.eof()) {
    // Reading position
    std::vector<float> p_vector(3, 0.0);
    for (int i = 0; i < 3; i++) {
      fileReader.read(reinterpret_cast<char *>(&p_vector[i]), sizeof(float));
    }

    // Determing position for each grid, put close to position reading for space locality
    std::vector<int> grid_position_vector(3, 0);
    for (int a = 0; a < 3; a++) {
      int position =
          static_cast<int>(std::floor((p_vector[a] - constants::min[a]) / block_size[a]));
      grid_position_vector[a] =
          (position >= grid_size[a]) ? grid_size[a] - 1 : ((position < 0) ? 0 : position);
    }

    // std::cout << grid_position_vector[0] << " " << grid_position_vector[1] << " "
    //           << grid_position_vector[2] << "\n";

    // Reading boundary
    std::vector<float> b_vector(3, 0.0);
    for (int i = 0; i < 3; i++) {
      fileReader.read(reinterpret_cast<char *>(&b_vector[i]), sizeof(float));
    }
    // Reading velocity
    std::vector<float> v_vector(3, 0.0);
    for (int i = 0; i < 3; i++) {
      fileReader.read(reinterpret_cast<char *>(&v_vector[i]), sizeof(float));
    }
    // Adding to particle list, converting float values to double values
    particles.push_back(particle{counter, std::vector<double>(p_vector.begin(), p_vector.end()),
                                 std::vector<double>(b_vector.begin(), b_vector.end()),
                                 std::vector<double>(v_vector.begin(), v_vector.end()),
                                 constants::acceleration, 0.0});

    // Adding to grid
    grid[grid_position_vector[0]][grid_position_vector[1]][grid_position_vector[2]].insert(counter);
    counter++;
  }
  particles.pop_back();

  if (particles.size() != np) {
    std::cerr << "Error: Number of particles mismatch. Header: " << np
              << ", "
                 "Found: "
              << particles.size() << ".\n";
    exit(-5);
  }

  std::cout << "Number of particles: " << np
            << "\n"
               "Particles per meter: "
            << ppm
            << "\n"
               "Smoothing length: "
            << blocks.smoothing_length_h
            << "\n"
               "Particle mass: "
            << blocks.mass
            << "\n"
               "Grid size: "
            << blocks.grid_size[0] << " x " << blocks.grid_size[1] << " x " << blocks.grid_size[2]
            << "\n"
               "Number of blocks: "
            << blocks.grid_size[0] * blocks.grid_size[1] * blocks.grid_size[2]
            << "\n"
               "Block size: "
            << block_size[0] << " x " << block_size[1] << " x " << block_size[2] << "\n";
}

bool blockExists(std::vector<int> const & nearby_block, std::vector<int> const & grid_size) {
  return ((nearby_block[0] < grid_size[0]) && (nearby_block[0] >= 0) &&
          (nearby_block[1] < grid_size[1]) && (nearby_block[1] >= 0) &&
          (nearby_block[2] < grid_size[2]) && (nearby_block[2] >= 0))
             ? true
             : false;
}

void updateNearbyBlocks(int particle_id, std::vector<int> const & nearby_block,
                        block_parameters const & blocks) {
  // Block i is this_particle and block j is nearby_particle

  if (blockExists(nearby_block, blocks.grid_size) == false) { return; }
  std::cout << "Nearby block exists: " << nearby_block[0] << " " << nearby_block[1] << " "
            << nearby_block[2] << "\n";

  if (grid[nearby_block[0]][nearby_block[1]][nearby_block[2]].empty()) {
    std::cout << "Empty at block: " << nearby_block[0] << " " << nearby_block[1] << " "
              << nearby_block[2] << "\n";
    return;
  }

  for (int nearby_particle_id : grid[nearby_block[0]][nearby_block[1]][nearby_block[2]]) {
    particle this_particle   = particles[particle_id];
    particle nearby_particle = particles[nearby_particle_id];

    double h6_val = pow(blocks.smoothing_length_h, 6);
    double squared_distance =
        pow(sqrt(pow(this_particle.position_vector[0] - nearby_particle.position_vector[0], 2) +
                 pow(this_particle.position_vector[1] - nearby_particle.position_vector[1], 2) +
                 pow(this_particle.position_vector[2] - nearby_particle.position_vector[2], 2)),
            2);
    bool blocks_close = (squared_distance < pow(blocks.smoothing_length_h, 2)) ? true : false;

    if (blocks_close && nearby_particle_id != particle_id) {
      // Densities
      double density_delta  = pow(pow(blocks.smoothing_length_h, 2) - squared_distance, 3);
      this_particle.density = density_delta;
      double density_transform_multiplication =
          (315.0 / (64.0 * std::numbers::pi * pow(blocks.smoothing_length_h, 9)) * blocks.mass);

      this_particle.density = (this_particle.density + h6_val) * density_transform_multiplication;
      nearby_particle.density += density_delta;
      nearby_particle.density =
          (nearby_particle.density + h6_val) * density_transform_multiplication;

      // Acceleration
      double dist = sqrt(std::max(squared_distance, 10E-12));
      double acceleration_delta =
          (((this_particle.position_vector[0] - nearby_particle.position_vector[0] +
             this_particle.position_vector[1] - nearby_particle.position_vector[1] +
             this_particle.position_vector[2] - nearby_particle.position_vector[2]) *
            (15.0 / (std::numbers::pi * h6_val)) * blocks.mass *
            (pow(blocks.smoothing_length_h - dist, 2) / dist) *
            (this_particle.density + nearby_particle.density - constants::rho)) +
           ((nearby_particle.velocity_vector[0] - this_particle.velocity_vector[0] +
             nearby_particle.velocity_vector[1] - this_particle.velocity_vector[1] +
             nearby_particle.velocity_vector[2] - this_particle.velocity_vector[2]) *
            (45.0 / (std::numbers::pi * h6_val)) * constants::mu * blocks.mass)) /
          this_particle.density * nearby_particle.density;
      for (int i = 0; i < 3; i++) {
        this_particle.acceleration_vector[i]   += acceleration_delta;
        nearby_particle.acceleration_vector[i] -= acceleration_delta;
      }
    }
  }
}

void simulationProcessing(block_parameters const & blocks) {
  // std::vector<std::vector<std::vector<std::set<int>>>>
  // grid;
  // NOTE: the idea to not replicate is to only consider combinations that add, no subtractions
  std::cout << "Checking grid size: " << grid.size() << "X" << grid[0].size() << "X"
            << grid[0][0].size() << "\n";
  for (int z = 0; z < blocks.grid_size[2]; z++) {
    for (int y = 0; y < blocks.grid_size[1]; y++) {
      for (int x = 0; x < blocks.grid_size[0]; x++) {
        // std::cout << "X: " << x << " Y: " << y << " Z: " << z << "\n";
        std::vector<int> this_block{x, y, z};
        for (int particle_id : grid[this_block[0]][this_block[1]][this_block[2]]) {
          // std::cout << particle_id << "\n";

          // To move up through the box, we'll move up through the bottom corner (<0,0,0>), x can
          // go down. We never go down for horizontal, since only positive or no changes accounts
          // for those, and we'll never go down in the z direction!

          // Possible Combinations:
          // Same block
          //    <x, y, z>
          // Horizontal(don't go negative)
          //    <x+1, y, z>
          //    <x, y+1, z>
          //    <x, y, z+1>
          // Diagonal:
          //    Level of 9 blocks below (z-1), we don't need to worry about because those are
          //    accounted for by the last level! On the same z level, we can 1) forget about
          //    horizontals 2) Only move in y direction, move x either down or up
          //        <x+1, y+1, z>
          //        <x-1, y+1, z>
          // On the upper z level, we'll move in the z direction and do either combination of x or
          // y
          //        <x+1, y, z+1>
          //        <x-1, y, z+1>
          //        <x+1, y+1, z+1>
          //        <x+1, y-1, z+1>
          //        <x-1, y+1, z+1>
          //        <x-1, y-1, z+1>

          // Passes in a nearbyBlock vector for locality/loops, maybe some optimizations to not
          // redeclare it
          // Same block (we know it exists)
          updateNearbyBlocks(particle_id, this_block, blocks);

          // Horizontal
          updateNearbyBlocks(particle_id, std::vector<int>{x + 1, y, z}, blocks);
          updateNearbyBlocks(particle_id, std::vector<int>{x, y + 1, z}, blocks);
          updateNearbyBlocks(particle_id, std::vector<int>{x, y, z + 1}, blocks);

          // Diagonal, same z level
          updateNearbyBlocks(particle_id, std::vector<int>{x + 1, y + 1, z}, blocks);
          updateNearbyBlocks(particle_id, std::vector<int>{x - 1, y + 1, z}, blocks);

          // Diagonal, upper z level
          updateNearbyBlocks(particle_id, std::vector<int>{x + 1, y, z + 1}, blocks);
          updateNearbyBlocks(particle_id, std::vector<int>{x - 1, y, z + 1}, blocks);
          updateNearbyBlocks(particle_id, std::vector<int>{x + 1, y + 1, z + 1}, blocks);
          updateNearbyBlocks(particle_id, std::vector<int>{x + 1, y - 1, z + 1}, blocks);
          updateNearbyBlocks(particle_id, std::vector<int>{x - 1, y + 1, z + 1}, blocks);
          updateNearbyBlocks(particle_id, std::vector<int>{x - 1, y - 1, z + 1}, blocks);
        }
      }
    }
  }
}

void testOutput(char * outputFile) {
  return;
}

void writeFile(std::string outputFile, float ppm, int np, std::vector<particle> particles) {
  std::ofstream fileWriter;
  fileWriter.open(outputFile, std::ios::binary);
  fileWriter.write(reinterpret_cast<char *>(&ppm), sizeof(ppm));
  fileWriter.write(reinterpret_cast<char *>(&np), sizeof(np));
  for (particle p : particles) {
    for (int i = 0; i < 3; i++) {
      fileWriter.write(reinterpret_cast<char *>(&p.position_vector[i]), sizeof(float));
    }
    for (int i = 0; i < 3; i++) {
      fileWriter.write(reinterpret_cast<char *>(&p.boundary_vector[i]), sizeof(float));
    }
    for (int i = 0; i < 3; i++) {
      fileWriter.write(reinterpret_cast<char *>(&p.velocity_vector[i]), sizeof(float));
    }
  }
}

/*
run with
g++ -std=c++11 -o fluid fluid.cpp
./fluid **timestep** **inputfile** **outputfile**
*/
int main(int argc, char * argv[]) {
  if (argc != 4) {
    std::cerr << "Error: Invalid number of arguments: " << argc - 1 << ".\n";
    return 1;
  }

  // Input
  int nts = parseInt(argv[1]);
  block_parameters blocks;
  parseInput(argv[2], blocks);

  // for (particle p : particles) {
  //   std::cout << p.id << " px: " << p.position_vector[0] << " py: " << p.position_vector[1]
  //             << " pz: " << p.position_vector[2] << "\n";
  // }

  // simulationProcessing(blocks);

  // testOutput(argv[3]);

  // // Testing, for some reason 7, 10, 7 is giving me a seg fault, I don't get it. It tells me
  // empty when I test in parseInput

  std::cout << "Checking grid size: " << grid.size() << "X" << grid[0].size() << "X"
            << grid[0][0].size() << "\n";
  std::vector<int> this_block{7, 10, 7};
  if (grid[this_block[0]][this_block[1]][this_block[2]].empty()) { std::cout << "TRUE"; }

  for (int particle_id : grid[this_block[0]][this_block[1]][this_block[2]]) {
    particle this_particle = particles[particle_id];
    std::cout << "ID: " << particle_id << " "
              << " Density: " << this_particle.density << " Acceleration"
              << this_particle.acceleration_vector[0] << " x "
              << this_particle.acceleration_vector[1] << " x "
              << this_particle.acceleration_vector[2] << "\n";
  }

  // std::vector<particle> segmented;
  // for (int i = 0; i < 4800; ++i) { segmented.push_back(particles[i]); }
  // writeFile("test.fld", 0, 4800, segmented);
  // std::cout << "Here";
}