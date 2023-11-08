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
    void repositionAndInitialize();
    static double geomNormSquared(std::vector<double> const & pos1,
                                  std::vector<double> const & pos2);
    void updateDensityBetweenParticles(particle & part1, particle & part2);
    void updateAccelerationBetweenParticles(particle & part1, particle & part2);
    void updateSameBlock(std::vector<int> const & pos, bool updateType);
    void updateDifferentBlock(std::vector<int> const & pos1, std::vector<int> const & pos2,
                              bool updateType);
    void increaseVal(bool updateType);
    void increaseSurroundingBlocks(int const & i, int const & j, int const & k, bool updateType);
    void densityTransform();
    void updateAccelerationWithWallMin(particle & part, int index);
    void updateAccelerationWithWallMax(particle & part, int index);
    void updateAccelerationWithWall(particle & part, std::vector<int> const & grid_position);
    static void particlesMotion(particle & part);
    void collideWithWallMin(particle & part, int index);
    void collideWithWallMax(particle & part, int index);
    void collideWithWall(particle & part, std::vector<int> const & grid_position);
    void processStep();
};

#endif  // GRID_HPP