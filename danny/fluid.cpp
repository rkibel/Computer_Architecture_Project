#include <iostream>
#include <vector>
#include <charconv>
#include <fstream>
#include <sstream>
#include <cmath>

#include <typeinfo>

struct particle_read {
    int id;
    double px;
    double py;
    double pz;
    double hvx;
    double hvy;
    double hvz;
    double vx;
    double vy;
    double vz;
};

struct constants {
    double radius_multiplier_r = 1.695;
    double fluid_density_rho = 1000;
    double stiffness_presure_ps = 3.0;
    double stiffness collisions_sc = 30000;
    double damping_dv = 128.0;
    double viscosity_mu = 0.4;
    double particle_size_dp= .0002;
    double time_step_delta_t = .001;
    vector<double> gravity_g{0.0, 9.8, 0.0};
    vector<double> box_upper_bound_bmin{0.065, 0.1, 0.065};    
    vector<double> box_lower_bound_bmax{-0.065, -0.08, -0.065};   
}

struct parameters {
    double particle_mass_m;
    double smoothing_length_h;
    vector<int> number_blocks_by_dim_n;
    vector<double> sizes_blocks_by_dim_s;
}

int parseInt(char* arg) {
    const std::string input_str = arg;
    int res;
    auto result = std::from_chars(input_str.data(), input_str.data() + input_str.size(), res);
    if (result.ec != std::errc()) {
        std::cerr << "Error: time steps must be numeric.\n";
        exit(1);
    }
    if (res < 0) {
        std::cerr << "Error: Invalid number of time steps.\n";
        exit(1);
    }
    return res;
}

std::vector<particle> parseInput(char* inputFile) {
    std::ifstream fileReader;
    fileReader.open(inputFile, std::ios::binary);
    if (!fileReader) {
        std::cerr << "Error: Cannot open " << inputFile << " for reading";
        exit(1);
    }
    std::vector<particle> particles;
    float ppm = 0.0;
    int np = 0;
    fileReader.read(reinterpret_cast<char*>(&ppm), sizeof(ppm));
    fileReader.read(reinterpret_cast<char*>(&np), sizeof(np));

    initializeParameters(static_cast<double>(ppm))

    for (unsigned int i = 0; i < np; ++i) {
        particle p;
        fileReader.read(reinterpret_cast<char*>(&p), 36);
        
        particles.push_back(p);
    }
    return particles;
}

void testOutput(char* outputFile) {
    return;
}

void initializeParameters(double ppm):
    parameters.particle_mass_m = constants.radius_multiplier_r / pow(ppm, 3);
    parameters.smoothing_length_h = constants.fluid_density_rho / ppm;
    for (int i = 0; i < 3; i++) {
        parameters.number_blocks_by_dim_n[i] = ( constants.box_upper_bound_bmin[i] - constants.box_upper_bound_bmin[i] ) / smoothing_length_h;
        parameters.sizes_blocks_by_dim_s[i] = ( constants.box_upper_bound_bmin[i] - constants.box_upper_bound_bmin[i] ) / smoothing_length_h;
    }



/*
run with
g++ -std=c++11 -o fluid fluid.cpp
./fluid **timestep** **inputfile** **outputfile**
*/
int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments: " << argc-1 << ".\n";
        return 1;
    }
    
    int nts = parseInt(argv[1]);
    
    std::vector<particle> particles = parseInput(argv[2]);


    for (particle part: particles) {
        std::cout << part.px << " " << part.py << " " << part.pz << " " << part.hvx << " " << part.hvy << " " << part.hvz << " " << part.vx << " " << part.vy << " " << part.vz << "\n";
        break;
    }
    testOutput(argv[3]);
    

    // float x = 3.14;
    // x = static_cast<int>(x);
    // std::cout << typeid(x).name() << std::endl;
    // // double y = static_cast<double>(x);
    // // std::cout << y << "\n";
    // // std::cout << typeid(y).name() << std::endl;
    return 0;
}