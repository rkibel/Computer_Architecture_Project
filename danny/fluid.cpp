#include "constants.hpp"

#include <charconv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <typeinfo>
#include <vector>

struct particle {
    int const id;
    // will resize these three vectors when reading
    std::vector<double> position_vector;
    std::vector<double> energy_vector;  // represents hv, which means energy
    std::vector<double> velocity_vector;

    // comparing id
    bool operator<(particle const & other) const { return id < other.id; }
};

struct simulation_blocks {
    double mass;
    double smoothing_length_h;
    std::vector<int> grid_size;
    std::vector<double> block_size;
};

// Putting following two outside of the simulation_blocks struct so entire struct doesn't need to
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

std::vector<particle> parseInput(char * inputFile, simulation_blocks & blocks) {
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

  blocks = simulation_blocks{mass, smoothing_length_h, grid_size, block_size};

  // Reading particles
  int counter = 0;
  std::vector<particle> particles;

  while (!fileReader.eof()) {
    std::vector<float> p_vector(3, 0.0);
    for (int i = 0; i < 3; i++) {
      fileReader.read(reinterpret_cast<char *>(&p_vector[i]), sizeof(float));
    }
    std::vector<float> e_vector(3, 0.0);
    for (int i = 0; i < 3; i++) {
      fileReader.read(reinterpret_cast<char *>(&e_vector[i]), sizeof(float));
    }
    std::vector<float> v_vector(3, 0.0);
    for (int i = 0; i < 3; i++) {
      fileReader.read(reinterpret_cast<char *>(&v_vector[i]), sizeof(float));
    }
    std::vector<double> p_vec(p_vector.begin(), p_vector.end());
    particles.push_back(particle{counter, std::vector<double>(p_vector.begin(), p_vector.end()),
                                 std::vector<double>(e_vector.begin(), e_vector.end()),
                                 std::vector<double>(v_vector.begin(), v_vector.end())});

    std::vector<int> grid_position_vector(3, 0);
    for (int a = 0; a < 3; a++) {
      int position =
          static_cast<int>(std::floor((p_vector[a] - constants::min[a]) / block_size[a]));
      grid_position_vector[a] =
          (position >= grid_size[a]) ? grid_size[a] - 1 : ((position < 0) ? 0 : position);
    }

    // not working, not sure why
    grid[grid_position_vector[0]][grid_position_vector[1]][grid_position_vector[2]].insert(counter);
    counter++;
  }
  std::cout << "Done\n";
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
            << blocks.block_size[0] << " x " << blocks.block_size[1] << " x "
            << blocks.block_size[2] << "\n";

  return particles;
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
      fileWriter.write(reinterpret_cast<char *>(&p.energy_vector[i]), sizeof(float));
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
  simulation_blocks blocks;
  double ppm;
  int nts                         = parseInt(argv[1]);
  std::vector<particle> particles = parseInput(argv[2], blocks);

  // for (particle p : particles) {
  //   std::cout << p.id << " px: " << p.position_vector[0] << " py: " << p.position_vector[1]
  //             << " pz: " << p.position_vector[2] << "\n";
  // }

  testOutput(argv[3]);

  // // Testing
  // std::vector<particle> segmented;
  // for (int i = 0; i < 4800; ++i) { segmented.push_back(particles[i]); }
  // writeFile("test.fld", 0, 4800, segmented);
  // std::cout << "Here";
  return 0;
}