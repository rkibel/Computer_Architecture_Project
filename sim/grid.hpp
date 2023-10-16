#ifndef FA8064C2_D456_401A_ADAA_C9AF507B53DD
#define FA8064C2_D456_401A_ADAA_C9AF507B53DD

#include "block.hpp"
#include "particle.hpp"
#include <vector>
#include <fstream>

struct grid {
    std::vector<std::vector<std::vector<block>>> part_grid;
    std::vector<particle> part_dict;

    grid(std::vector<int> grid_size);

    int readParticles(std::ifstream& fileReader);
    void addParticleToGrid(int xval, int yval, int zval, particle part);
};

#endif /* FA8064C2_D456_401A_ADAA_C9AF507B53DD */
