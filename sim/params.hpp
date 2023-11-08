#ifndef PARAMS_HPP
#define PARAMS_HPP

#include "constants.hpp"
#include "utility.hpp"

#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>

struct params {
    double ppm              = 0.0;
    std::size_t np          = 0;
    double mass             = 0.0;
    double smoothing_length = 0.0;

    // factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
    std::vector<double> density_factors;
    // factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }
    std::vector<double> acceleration_factors;

    std::vector<int> grid_size;
    std::vector<double> block_size;
    std::vector<double> acceleration;
    std::vector<double> min;
    std::vector<double> max;

    void initialize(std::istream & fileReader);
    void initializeVectors();
    void initializeFactors();
};

#endif  // PARAMS_HPP
