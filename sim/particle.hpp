#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector>
#include <iostream>
#include <cmath>

struct particle {
    int id{};
    std::vector<double> position;
    std::vector<double> boundary;
    std::vector<double> velocity;
    std::vector<double> acceleration;
    double density{};
    std::vector<int> grid_pos;

    bool operator==(const particle& other) const;
    friend std::ostream& operator<<(std::ostream& os, const particle& part);
};

#endif  // PARTICLE_HPP
