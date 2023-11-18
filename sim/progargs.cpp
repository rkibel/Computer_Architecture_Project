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
  if (result.ec != std::errc() || result.ptr != str_view.data() + str_view.size()) {
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
    std::cerr << "Error: Cannot open " << inputFile << " for reading\n";
    exit(-3);
  }
  grid particle_grid(fileReader);
  params const parameters = particle_grid.parameters;
  if (particle_grid.part_dict.size() != parameters.np) {
    std::cerr << "Error: Number of particles mismatch. Header: " << parameters.np
              << ", Found: " << particle_grid.part_dict.size() << ".\n";
    int const exit_status_error = -5;
    exit(exit_status_error);
  }
  printGridInformation(parameters);
  return particle_grid;
}

void printGridInformation(params const & parameters) {
  std::cout << "Number of particles: " << parameters.np
            << "\n"
               "Particles per meter: "
            << parameters.ppm
            << "\n"
               "Smoothing length: "
            << parameters.smoothing_length
            << "\n"
               "Particle mass: "
            << parameters.mass
            << "\n"
               "Grid size: "
            << parameters.grid_size[0] << " x " << parameters.grid_size[1] << " x "
            << parameters.grid_size[2]
            << "\n"
               "Number of blocks: "
            << parameters.grid_size[0] * parameters.grid_size[1] * parameters.grid_size[2]
            << "\n"
               "Block size: "
            << parameters.block_size[0] << " x " << parameters.block_size[1] << " x "
            << parameters.block_size[2] << "\n";
}

void writeFile(std::string const & outputFile, float ppm, int np,
               std::vector<particle> const & particles) {
  std::ofstream fileWriter;
  fileWriter.open(outputFile, std::ios::binary);
  if (!fileWriter) {
    std::cerr << "Error: Cannot open " << outputFile << " for writing\n";
    exit(-4);
  }
  write_float(ppm, fileWriter);
  write_int(np, fileWriter);
  for (particle const & part : particles) {
    for (double const & iter : part.position) { write_float(static_cast<float>(iter), fileWriter); }
    for (double const & iter : part.boundary) { write_float(static_cast<float>(iter), fileWriter); }
    for (double const & iter : part.velocity) { write_float(static_cast<float>(iter), fileWriter); }
  }
}