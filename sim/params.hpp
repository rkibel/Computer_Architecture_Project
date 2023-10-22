#ifndef PARAMS_HPP
#define PARAMS_HPP

#include "constants.hpp"

#include <vector>
#include <iostream>
#include <numbers>
#include <cmath>

struct params {
    double ppm;
    std::size_t np;
    double mass;
    double smoothing_length;

    std::vector<double> density_factors; // factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
    std::vector<double> acceleration_factors; // factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }

    std::vector<int> grid_size;
    std::vector<double> block_size;

    void initialize(std::istream& fileReader);
};

#endif // PARAMS_HPP
