#ifndef FA8064C2_D456_401A_ADAA_C9AF507B53DD
#define FA8064C2_D456_401A_ADAA_C9AF507B53DD

#include "block.hpp"
#include "particle.hpp"
#include "params.hpp"
#include "constants.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

struct grid {
    std::vector<std::vector<std::vector<block>>> part_grid;
    std::vector<particle> part_dict;
    params parameters;

    grid(std::istream& fileReader);
    void repositionParticles();
    void initializeDensityAndAcceleration();
    double geomNormSquared(const std::vector<double> pos1, const std::vector<double> pos2);
    void updateDensityBetweenParticles(particle& part1, particle& part2);
    void updateAccelerationBetweenParticles(particle& part1, particle& part2);
    void updateSameBlock(std::vector<int> pos, bool updateType);
    void updateDifferentBlock(std::vector<int> pos1, std::vector<int> pos2, bool updateType);
    void increaseVal(bool updateType);
    void densityTransform();
    void updateAccelerationWithWallMin(particle& part, int index);
    void updateAccelerationWithWallMax(particle& part, int index);
    void updateAccelerationWithWall(particle& part, int x, int y, int z);
    void particlesMotion(particle& part);
    void collideWithWallMin(particle& part, int index);
    void collideWithWallMax(particle& part, int index);
    void collideWithWall(particle& part, int x, int y, int z);
    void processStep();
};

#endif /* FA8064C2_D456_401A_ADAA_C9AF507B53DD */
