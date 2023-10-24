#include "progargs.hpp"

void checkArgNumber(int argc) {
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments: " << argc-1 << ".\n";
        exit(-1);
    }
}

int parseInt(std::string arg) {
    int res{};
    auto result = std::from_chars(arg.data(), arg.data() + arg.size(), res);
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

grid parseInputFile(std::string inputFile) {
    std::ifstream fileReader;
    fileReader.open(inputFile, std::ios::binary);
    if (!fileReader) {
        std::cerr << "Error: Cannot open " << inputFile << " for reading";
        exit(-3);
    }
    grid particle_grid(fileReader);
    params parameters = particle_grid.parameters;
    if (particle_grid.part_dict.size() != parameters.np) {
        std::cerr << "Error: Number of particles mismatch. Header: " << parameters.np << ", Found: " << particle_grid.part_dict.size() << ".\n";
        exit(-5);
    }
    std::cout << "Number of particles: " << parameters.np << "\n"
    "Particles per meter: " << parameters.ppm << "\n"
    "Smoothing length: " << parameters.smoothing_length << "\n"
    "Particle mass: " << parameters.mass << "\n"
    "Grid size: " << parameters.grid_size[0] << " x " << parameters.grid_size[1] << " x " << parameters.grid_size[2] << "\n"
    "Number of blocks: " << parameters.grid_size[0] * parameters.grid_size[1] * parameters.grid_size[2] << "\n"
    "Block size: " << parameters.block_size[0] << " x " << parameters.block_size[1] << " x " << parameters.block_size[2] << "\n";

    return particle_grid;
}

void writeFile(std::string outputFile, float ppm, int np, std::vector<particle> particles) {
    std::ofstream fileWriter;
    fileWriter.open(outputFile, std::ios::binary);
    if (!fileWriter) {
        std::cerr << "Error: Cannot open " << outputFile << " for writing";
        exit(-4);
    }
    write_float(ppm, fileWriter);
    write_int(np, fileWriter);
    for (particle p: particles) {
        for (int i = 0; i < 9; ++i) {
            float temp = 0.0;
            if (i < 3) temp = p.position[i%3];
            else if (i < 6) temp = p.boundary[i%3];
            else temp = p.velocity[i%3];
            write_float(temp, fileWriter);
        }
    }
}