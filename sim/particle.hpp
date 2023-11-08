#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector>

struct particle {
    int id = 0;
    std::vector<double> position;
    std::vector<double> boundary;
    std::vector<double> velocity;
    std::vector<double> acceleration;
    double density = 0.0;
    std::vector<int> grid_pos{0, 0, 0};
};

#endif  // PARTICLE_HPP
