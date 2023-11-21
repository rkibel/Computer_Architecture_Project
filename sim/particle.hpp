#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <cmath>
#include <iostream>
#include <vector>

struct particle {
    int id{};
    std::vector<double> position;
    std::vector<double> boundary;
    std::vector<double> velocity;
    std::vector<double> acceleration;
    double density{};
    std::vector<int> grid_pos;

    bool operator==(particle const & other) const;
    friend std::ostream & operator<<(std::ostream & os, particle const & part);
};

#endif  // PARTICLE_HPP
