#ifndef GRID_HPP
#define GRID_HPP

#include "block.hpp"
#include "constants.hpp"
#include "params.hpp"
#include "particle.hpp"
#include "utility.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

struct grid {
    std::vector<std::vector<std::vector<block>>> part_grid;
    std::vector<particle> part_dict;
    params parameters;

    grid(std::istream & fileReader);
    void repositionParticles();
    void initializeDensityAndAcceleration();
    static double geomNormSquared(std::vector<double> const & pos1,
                                  std::vector<double> const & pos2);
    void updateDensityBetweenParticles(particle & part1, particle & part2);
    void updateAccelerationBetweenParticles(particle & part1, particle & part2);
    void updateSameBlock(std::vector<int> pos, bool updateType);
    void updateDifferentBlock(std::vector<int> pos1, std::vector<int> pos2, bool updateType);
    void increaseVal(bool updateType);
    void densityTransform();
    void updateAccelerationWithWallMin(particle & part, int index);
    void updateAccelerationWithWallMax(particle & part, int index);
    void updateAccelerationWithWall(particle & part, int x, int y, int z);
    void particlesMotion(particle & part);
    void collideWithWallMin(particle & part, int index);
    void collideWithWallMax(particle & part, int index);
    void collideWithWall(particle & part, int x, int y, int z);
    void processStep();
};

#endif  // GRID_HPP