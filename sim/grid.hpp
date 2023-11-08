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

    std::vector<std::pair<std::vector<int> const, int>>
        grid_combinations;  // Grid positions and number for "dictionary" of neighboring positions
    std::vector<std::vector<std::vector<int>>>
        grid_neighbor_combinations;  // "Dictionary" of neighboring positions

    grid(std::istream & fileReader);
    void addNeighborCombination(std::vector<std::vector<int>> & neighbor_combinations,
                                std::vector<int> const & neighbor_pos);
    void addNeighborCombinationVector(int const & i, int const & j, int const & k,
                                      std::vector<std::vector<int>> & neighbor_combinations);
    void initializeNeighborCombinations();
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