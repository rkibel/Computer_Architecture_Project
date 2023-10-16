#include "progargs.hpp"
#include "params.hpp"
#include "grid.hpp"

#include <iostream>
#include <charconv>
#include <fstream>

void checkArgNumber(int argc) {
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments: " << argc-1 << ".\n";
        exit(1);
    }
}

int parseInt(std::string arg) {
    int res{};
    auto result = std::from_chars(arg.data(), arg.data() + arg.size(), res);
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

void parseInputFile(std::string inputFile) {
    std::ifstream fileReader;
    fileReader.open(inputFile, std::ios::binary);
    if (!fileReader) {
        std::cerr << "Error: Cannot open " << inputFile << " for reading";
        exit(1);
    }

    params parameters(fileReader);
    grid g(parameters.grid_size);
    int counter = g.readParticles(fileReader);

    if (counter-1 != parameters.np) {
        std::cerr << "Error: Number of particles mismatch. Header: " << parameters.np << ", Found: " << counter-1 << ".\n";
        exit(-5);
    }

    std::cout << "Number of particles: " << parameters.np << "\n"
    "Particles per meter: " << parameters.ppm << "\n"
    "Smoothing length: " << parameters.smoothing_length << "\n"
    "Particle mass: " << parameters.mass << "\n"
    "Grid size: " << parameters.grid_size[0] << " x " << parameters.grid_size[1] << " x " << parameters.grid_size[2] << "\n"
    "Number of blocks: " << parameters.grid_size[0] * parameters.grid_size[1] * parameters.grid_size[2] << "\n"
    "Block size: " << parameters.block_size[0] << " x " << parameters.block_size[1] << " x " << parameters.block_size[2] << "\n";
}