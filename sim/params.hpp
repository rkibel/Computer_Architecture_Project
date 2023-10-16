#ifndef A1F0BE24_A80C_409A_AB77_4D13507013B7
#define A1F0BE24_A80C_409A_AB77_4D13507013B7

#include <vector>
#include <fstream>

struct params {
    double ppm;
    int np;
    double mass;
    double smoothing_length;

    std::vector<double> density_factors; // factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
    std::vector<double> acceleration_factors; // factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }

    std::vector<int> grid_size;
    std::vector<double> block_size;

    params(std::ifstream& fileReader);
};

#endif /* A1F0BE24_A80C_409A_AB77_4D13507013B7 */
