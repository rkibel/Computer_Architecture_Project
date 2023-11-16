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
    std::vector<std::vector<std::vector<int>>> grid_neighbor_combinations;

    grid(std::istream & fileReader);
    [[nodiscard]] bool isOutsideGrid(int i, int j, int k) const;
    [[nodiscard]] std::vector<std::vector<int>> getNeighbors(int i, int j, int k) const;
    void initializeNeighborCombinations();
    void repositionAndInitialize();
    static double normSquared(std::vector<double> const & pos1, std::vector<double> const & pos2);
    void updateDensity(particle & part1, particle & part2);
    void updateAcceleration(particle & part1, particle & part2);
    void updateBlock(std::vector<int> const & pos1, std::vector<int> const & pos2, bool type);
    void updateAccWithWall(particle & part);
    static void particlesMotion(particle & part);
    void collideWithWall(particle & part);
    void processStep();
};

#endif  // GRID_HPP