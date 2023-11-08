#include "progargs.hpp"

void checkArgNumber(int argc) {
  if (argc != 4) {
    std::cerr << "Error: Invalid number of arguments: " << argc - 1 << ".\n";
    exit(-1);
  }
}

int parseInt(std::string const & arg) {
  int res{};
  std::string_view const str_view(arg);
  auto result = std::from_chars(str_view.data(), str_view.data() + str_view.size(), res);
  if (result.ec != std::errc()) {
    std::cerr << "Error: time steps must be numeric.\n";
    exit(-1);
  }
  if (res < 0) {
    std::cerr << "Error: Invalid number of time steps.\n";
    exit(-2);
  }
  return res;
}

grid parseInputFile(std::string const & inputFile) {
  std::ifstream fileReader;
  fileReader.open(inputFile, std::ios::binary);
  if (!fileReader) {
    std::cerr << "Error: Cannot open " << inputFile << " for reading";
    exit(-3);
  }
  const grid grid(fileReader);
  if (grid.part_dict.size() != grid.np) {
    std::cerr << "Error: Number of particles mismatch. Header: " << grid.np
              << ", Found: " << grid.part_dict.size() << ".\n";
    int const exit_status_error = -5;
    exit(exit_status_error);
  }
  printGridInformation(grid);
  return grid;
}

void printGridInformation(grid const & grid) {
  std::cout << "Number of particles: " << grid.np << "\n"
               "Particles per meter: " << grid.ppm << "\n"
               "Smoothing length: " << grid.smoothing_length << "\n"
               "Particle mass: " << grid.mass << "\n"
               "Grid size: " << grid.grid_size[0] << " x " << grid.grid_size[1] << " x " 
               << grid.grid_size[2] << "\n"
               "Number of blocks: " << grid.grid_size[0] * grid.grid_size[1] * 
               grid.grid_size[2] << "\n"
               "Block size: " << grid.block_size[0] << " x " << grid.block_size[1] << " x "
               << grid.block_size[2] << "\n";
}

void writeFile(std::string const & outputFile, grid const & grid) {
  std::ofstream fileWriter;
  fileWriter.open(outputFile, std::ios::binary);
  if (!fileWriter) {
    std::cerr << "Error: Cannot open " << outputFile << " for writing";
    exit(-4);
  }
  const auto write_ppm = static_cast<float>(grid.ppm);
  const auto write_np = static_cast<int>(grid.np);
  write_float(write_ppm, fileWriter);
  write_int(write_np, fileWriter);
  for (const particle & part : grid.part_dict) {
    for (double const & iter : part.position){ write_float(static_cast<float>(iter), fileWriter); }
    for (double const & iter : part.boundary){ write_float(static_cast<float>(iter), fileWriter); }
    for (double const & iter : part.velocity){ write_float(static_cast<float>(iter), fileWriter); }    
  }
}