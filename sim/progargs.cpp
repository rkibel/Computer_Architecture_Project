#include "progargs.hpp"

void checkArgNumber(int argc) {
  if (argc != 4) {
    std::cerr << "Error: Invalid number of arguments: " << argc - 1 << ".\n";
    exit(-1);
  }
}

int parseInt(std::string const & arg) {
  int res{};
  std::string_view const str_view(arg);  // Creating a string_view from the string
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
  int const exit_status_error = -5;  // Magic number
  std::ifstream fileReader;
  fileReader.open(inputFile, std::ios::binary);
  if (!fileReader) {
    std::cerr << "Error: Cannot open " << inputFile << " for reading";
    exit(-3);
  }
  grid particle_grid(fileReader);
  params const parameters = particle_grid.parameters;
  if (particle_grid.part_dict.size() != parameters.np) {
    std::cerr << "Error: Number of particles mismatch. Header: " << parameters.np
              << ", Found: " << particle_grid.part_dict.size() << ".\n";
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
    std::cerr << "Error: Cannot open " << outputFile << " for writing";
    exit(-4);
  }
  write_float(ppm, fileWriter);
  write_int(np, fileWriter);
  int const line_values       = 9;
  int const pos_conditional   = 3;
  int const bound_conditional = 6;
  for (particle part : particles) {
    for (int i = 0; i < line_values; ++i) {
      auto temp = static_cast<float>(part.velocity[i % 3]);
      if (i < pos_conditional) {
        temp = static_cast<float>(part.position[i % 3]);
      } else if (i < bound_conditional) {
        temp = static_cast<float>(part.boundary[i % 3]);
      }
      write_float(temp, fileWriter);
    }
  }
}