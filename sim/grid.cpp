#include "grid.hpp"
#include "utility.cpp"

grid::grid(std::vector<int> grid_size) {
    part_grid.resize(grid_size[0], std::vector<std::vector<block>>(grid_size[1], std::vector<block>(grid_size[2])));
}

int grid::readParticles(std::ifstream& fileReader) {
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
    return counter;
}

void grid::addParticleToGrid(int xval, int yval, int zval, particle part) {
    part_grid[xval][yval][zval].push_back(part);
}