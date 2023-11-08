#ifndef GRID_HPP
#define GRID_HPP

#include "block.hpp"
#include "particle.hpp"
#include "utility.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <numbers>

namespace constants {
  double const radius_mult      = 1.695;
  double const fluid_density    = 1e3;
  double const stiff_pressure   = 3.0;
  double const stiff_collisions = 3e4;
  double const damping          = 128.0;
  double const viscosity        = 0.4;
  double const particle_size    = 2e-4;
  double const delt_t           = 1e-3;
}

struct grid {
    std::vector<std::vector<std::vector<block>>> part_grid;
    std::vector<particle> part_dict;
    double ppm{};
    std::size_t np = 0;
    double mass{};
    double smoothing_length{};

    // factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
    std::vector<double> density_factors;
    // factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }
    std::vector<double> acceleration_factors;

    std::vector<int> grid_size;
    std::vector<double> block_size;
    std::vector<double> acceleration;
    std::vector<double> min;
    std::vector<double> max;

    std::vector<std::pair<std::vector<int> const, int>>
        grid_combinations;  // Grid positions and number for "dictionary" of neighboring positions
    std::vector<std::vector<std::vector<int>>>
        grid_neighbor_combinations;  // "Dictionary" of neighboring positions

    grid(std::istream & fileReader);
    void initializeVectors();
    void initializeFactors();
    void initializeParticles(std::istream & fileReader);
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