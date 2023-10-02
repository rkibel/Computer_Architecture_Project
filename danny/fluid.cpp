#include "constants.hpp"

#include <charconv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>

struct particle {
    int const id;
    double px;
    double py;
    double pz;
    double hvx;
    double hvy;
    double hvz;
    double vx;
    double vy;
    double vz;
};

struct simulation_blocks {
    double mass;
    double smoothing_length_h;
    std::vector<int> grid_size;
    std::vector<double> block_size;
};

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

  double const ppm = static_cast<double>(ppm_read);

  int counter = 0;
  std::vector<particle> particles;
  while (!fileReader.eof()) {
    float px, py, pz, hvx, hvy, hvz, vx, vy, vz;
    fileReader.read(reinterpret_cast<char *>(&px), sizeof(float));
    fileReader.read(reinterpret_cast<char *>(&py), sizeof(float));
    fileReader.read(reinterpret_cast<char *>(&pz), sizeof(float));
    fileReader.read(reinterpret_cast<char *>(&hvx), sizeof(float));
    fileReader.read(reinterpret_cast<char *>(&hvy), sizeof(float));
    fileReader.read(reinterpret_cast<char *>(&hvz), sizeof(float));
    fileReader.read(reinterpret_cast<char *>(&vx), sizeof(float));
    fileReader.read(reinterpret_cast<char *>(&vy), sizeof(float));
    fileReader.read(reinterpret_cast<char *>(&vz), sizeof(float));
    particles.push_back(particle{counter, px, py, pz, hvx, hvy, hvz, vx, vy, vz});
    counter++;
  }
  particles.pop_back();
  ;

  if (particles.size() != np) {
    std::cerr << "Error: Number of particles mismatch. Header: " << np
              << ", "
                 "Found: "
              << particles.size() << ".\n";
    exit(-5);
  }

  double mass               = constants::rho / ppm / ppm / ppm;
  double smoothing_length_h = constants::r / ppm;
  std::vector<int> grid_size{
      static_cast<int>(std::floor((constants::max[0] - constants::min[0]) / smoothing_length_h)),
      static_cast<int>(std::floor((constants::max[1] - constants::min[1]) / smoothing_length_h)),
      static_cast<int>(std::floor((constants::max[2] - constants::min[2]) / smoothing_length_h))};

  std::vector<double> block_size{(constants::max[0] - constants::min[0]) / grid_size[0],
                                 (constants::max[1] - constants::min[0]) / grid_size[1],
                                 (constants::max[2] - constants::min[0]) / grid_size[2]};

  blocks = simulation_blocks{mass, smoothing_length_h, grid_size, block_size};

  // solely for testing purposes
  //  /*
  for (int i = 0; i < particles.size(); i++) {
    particle p = particles[i];
    std::cout << p.id << ": " << p.px << " " << p.py << " " << p.pz << "\n";
    break;
  }
  // */

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
  std::ofstream file;
  file.open(outputFile, std::ios::binary);
  file.write(reinterpret_cast<char *>(&ppm), sizeof(ppm));
  file.write(reinterpret_cast<char *>(&np), sizeof(np));
  for (particle p : particles) {
    float px  = p.px;
    float py  = p.py;
    float pz  = p.pz;
    float hvx = p.hvx;
    float hvy = p.hvy;
    float hvz = p.hvz;
    float vx  = p.vx;
    float vy  = p.vy;
    float vz  = p.vz;
    file.write(reinterpret_cast<char *>(&px), sizeof(float));
    file.write(reinterpret_cast<char *>(&py), sizeof(float));
    file.write(reinterpret_cast<char *>(&pz), sizeof(float));
    file.write(reinterpret_cast<char *>(&hvx), sizeof(float));
    file.write(reinterpret_cast<char *>(&hvy), sizeof(float));
    file.write(reinterpret_cast<char *>(&hvz), sizeof(float));
    file.write(reinterpret_cast<char *>(&vx), sizeof(float));
    file.write(reinterpret_cast<char *>(&vy), sizeof(float));
    file.write(reinterpret_cast<char *>(&vz), sizeof(float));
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
  testOutput(argv[3]);

  // Testing
  std::vector<particle> segmented;
  for (int i = 0; i < 4800; ++i) { segmented.push_back(particles[i]); }
  writeFile("test.fld", 0, 4800, segmented);
  std::cout << "Here";
  return 0;
}