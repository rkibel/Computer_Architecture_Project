#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector>

struct particle {
    int id;
    std::vector<double> position;
    std::vector<double> boundary;
    std::vector<double> velocity;
    std::vector<double> acceleration;
    double density;
};

#endif // PARTICLE_HPP
