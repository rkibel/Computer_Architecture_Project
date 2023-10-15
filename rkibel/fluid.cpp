#include <iostream>
#include <vector>
#include <set>
#include <charconv>
#include <fstream>
#include <sstream>
#include <cmath>
#include <numbers>
#include <span>
#include "constants.hpp"
#include "utility.cpp"

struct particle {
    int id;
    std::vector<double> position;
    std::vector<double> boundary;
    std::vector<double> velocity;
    std::vector<double> acceleration;
    std::vector<int> grid_positioning;
    double density;

    bool operator<(particle const & other) const { return id < other.id; }
};

template <typename T> requires(std::is_integral_v<T> or std::is_floating_point_v<T>)
char * as_writable_buffer(T & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char *>(&value);
}

template <typename T> requires(std::is_integral_v<T> or std::is_floating_point_v<T>)
T read_binary_value(std::istream & is) {
  T value{};
  is.read(as_writable_buffer(value), sizeof(value));
  return value;
}


template <typename T> requires(std::is_integral_v<T> or std::is_floating_point_v<T>)
char const * as_buffer(T const & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char const *>(&value);
}

template <typename T> requires(std::is_integral_v<T> or std::is_floating_point_v<T>)
void write_binary_value(T value, std::ostream & os) {
  os.write(as_buffer(value), sizeof(value));
}

double ppm;
int np;
double mass;
double smoothing_length;
std::vector<int> grid_size;
std::vector<double> block_size;
std::vector<double> density_factors; // factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
std::vector<double> acceleration_factors; // factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }

std::vector<particle> particles; // think of this as a dictionary of the particles
std::vector<std::vector<std::vector<std::set<int>>>> grid; // store only the particle ids in the grid

int parseInt(std::string arg) {
    int res;
    auto result = std::from_chars(arg.data(), arg.data() + arg.size(), res);
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

void initializeFactors(double ppm) {
    mass = constants::fluid_density / ppm / ppm / ppm;
    smoothing_length = constants::radius_mult / ppm;
    density_factors = {
        smoothing_length*smoothing_length, 
        std::pow(smoothing_length, 6), 
        315.0 * mass / 64.0 / std::numbers::pi / std::pow(smoothing_length, 9)
    };
    acceleration_factors = {
        smoothing_length * smoothing_length,
        45.0 * mass * constants::stiff_pressure / std::numbers::pi / std::pow(smoothing_length, 6) / 2,
        45.0 * mass * constants::viscosity / std::numbers::pi / std::pow(smoothing_length, 6)
    };
}

void parseInput(std::string inputFile) {
    std::ifstream fileReader;
    fileReader.open(inputFile, std::ios::binary);
    if (!fileReader) {
        std::cerr << "Error: Cannot open " << inputFile << " for reading";
        exit(1);
    }

    ppm = read_binary_value<float>(fileReader);
    np = read_binary_value<int>(fileReader);
    if (np <= 0) {
        std::cerr << "Error: Invalid number of particles: " << np << ".\n";
        exit(-5);
    }

    initializeFactors(ppm);
    grid_size = {static_cast<int>(std::floor((constants::max[0] - constants::min[0]) / smoothing_length)), 
                 static_cast<int>(std::floor((constants::max[1] - constants::min[1]) / smoothing_length)),
                 static_cast<int>(std::floor((constants::max[2] - constants::min[2]) / smoothing_length))};
    block_size = {(constants::max[0] - constants::min[0]) / grid_size[0], 
                  (constants::max[1] - constants::min[1]) / grid_size[1],
                  (constants::max[2] - constants::min[2]) / grid_size[2]};
    grid.resize(grid_size[0], std::vector<std::vector<std::set<int>>>(grid_size[1], std::vector<std::set<int>>(grid_size[2])));

    int counter = 0;
    while (!fileReader.eof()) {
        particle p;
        p.id = counter;
        for (int i = 0; i < 9; ++i) {
            float temp = read_binary_value<float>(fileReader);
            if (i < 3) p.position.push_back(temp);
            else if (i < 6) p.boundary.push_back(temp);
            else p.velocity.push_back(temp);
        }
        particles.push_back(p);
        counter++;
    }
    particles.pop_back();

    if (counter-1 != np) {
        std::cerr << "Error: Number of particles mismatch. Header: " << np << ", Found: " << counter-1 << ".\n";
        exit(-5);
    }


    std::cout << "Number of particles: " << np << "\n"
    "Particles per meter: " << ppm << "\n"
    "Smoothing length: " << smoothing_length << "\n"
    "Particle mass: " << mass << "\n"
    "Grid size: " << grid_size[0] << " x " << grid_size[1] << " x " << grid_size[2] << "\n"
    "Number of blocks: " << grid_size[0] * grid_size[1] * grid_size[2] << "\n"
    "Block size: " << block_size[0] << " x " << block_size[1] << " x " << block_size[2] << "\n";
}

void repositionParticles() {
    for (unsigned int i = 0; i < particles.size(); ++i) {
        std::vector<int> orgpos = particles[i].grid_positioning;
        std::vector<int> newpos;
        for (int j = 0; j < 3; ++j) {
            int position = static_cast<int>(std::floor((particles[i].position[j] - constants::min[j]) / block_size[j]));
            newpos.push_back(std::max(0, std::min(position, grid_size[j]-1))); 
        }
        if (newpos != orgpos) {
            if (orgpos.size() == 3) grid[orgpos[0]][orgpos[1]][orgpos[2]].erase(i);
            grid[newpos[0]][newpos[1]][newpos[2]].insert(i);
            particles[i].grid_positioning = newpos;
        }
    }
}

void initializeDensityAndAcceleration() {
    for (particle& part: particles) {
        part.density = 0.0;
        part.acceleration = constants::acceleration;
    }
}

double geomNormSquared(const std::vector<double> pos1, const std::vector<double> pos2) {
    return std::pow(pos1[0] - pos2[0], 2) + 
           std::pow(pos1[1] - pos2[1], 2) + 
           std::pow(pos1[2] - pos2[2], 2);
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void updateDensityBetweenParticles(particle& part1, particle& part2) {
    double normSquared = geomNormSquared(part1.position, part2.position);
    if (normSquared < density_factors[0]) {
        double densityIncrease = std::pow(density_factors[0] - normSquared, 3);
        part1.density += densityIncrease;
        part2.density += densityIncrease;
    }
}

// factors = { h^2, 45*m*p_s/pi/h^6/2 , 45*mu*m/pi/h^6 }
void updateAccelerationBetweenParticles(particle& part1, particle& part2) {
    double normSquared = geomNormSquared(part1.position, part2.position);
    if (normSquared < acceleration_factors[0]) {
        double dist = std::sqrt(std::max(normSquared, 1e-12));
        for (int i = 0; i < 3; ++i) {
            double delta_a =
                (part1.position[i] - part2.position[i]) *
                acceleration_factors[1] * \
                (std::pow(smoothing_length - dist, 2) / dist) * \
                (part1.density + part2.density - 2.0 * constants::fluid_density);
            delta_a += (part2.velocity[i] - part1.velocity[i]) * acceleration_factors[2];
            delta_a /= (part1.density * part2.density);
            part1.acceleration[i] += delta_a;
            part2.acceleration[i] -= delta_a;
        }
    }
}

// updateType = true: update density
// updateType = false: update acceleration
void updateSameBlock(std::vector<int> pos, bool updateType) {
    std::set<int> particleSet = grid[pos[0]][pos[1]][pos[2]];
    for (auto it1 = particleSet.begin(); it1 != particleSet.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != particleSet.end(); ++it2) {
            if (updateType) updateDensityBetweenParticles(particles[*it1], particles[*it2]);
            else updateAccelerationBetweenParticles(particles[*it1], particles[*it2]);
        }
    }
}

void updateDifferentBlock(std::vector<int> pos1, std::vector<int> pos2, bool updateType) {
    if (pos1[0] >= grid_size[0] || pos1[0] < 0 ||
        pos1[1] >= grid_size[1] || pos1[1] < 0 ||
        pos1[2] >= grid_size[2] || pos1[2] < 0 ||
        pos2[0] >= grid_size[0] || pos2[0] < 0 ||
        pos2[1] >= grid_size[1] || pos2[1] < 0 ||
        pos2[2] >= grid_size[2] || pos2[2] < 0) return; 
    
    std::set<int> set1 = grid[pos1[0]][pos1[1]][pos1[2]];
    std::set<int> set2 = grid[pos2[0]][pos2[1]][pos2[2]];
    for (auto it1 = set1.begin(); it1 != set1.end(); ++it1) {
        for (auto it2 = set2.begin(); it2 != set2.end(); ++it2) {
            if (updateType) updateDensityBetweenParticles(particles[*it1], particles[*it2]);
            else updateAccelerationBetweenParticles(particles[*it1], particles[*it2]);
        }
    }
}

// updateType = true: update density
// updateType = false: update acceleration
void increaseVal(bool updateType) {
    for (int i = 0; i < grid_size[0]; ++i) {
        for (int j = 0; j < grid_size[1]; ++j) {
            for (int k = 0; k < grid_size[2]; ++k) {
                updateSameBlock(std::vector<int>{i, j, k}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j+1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j+1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i-1, j, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i-1, j-1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i-1, j+1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j-1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j-1, k+1}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j, k}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i+1, j+1, k}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i, j+1, k}, updateType);
                updateDifferentBlock(std::vector<int>{i, j, k}, std::vector<int>{i-1, j+1, k}, updateType);
            }
        }
    }
}

// factors = { h^2, h^6, 315/64 * mass / pi / h^9 }
void densityTransform() {
    for (particle& part: particles) {
        part.density = (part.density + density_factors[1]) * density_factors[2];
    }
}

void updateAccelerationWithWall(particle& part, int index) {
    if (part.grid_positioning[index] == 0 || part.grid_positioning[index] == grid_size[index]-1) {
        double newcoord = part.position[index] + part.boundary[index] * constants::delt_t;
        double delt = (part.grid_positioning[index] == 0) ?
            constants::particle_size - (newcoord - constants::min[index]) :
            constants::particle_size - (constants::max[index] - newcoord);
        if (delt > 1e-10) {
            if (part.grid_positioning[index] == 0) 
                part.acceleration[index] += constants::stiff_collisions * delt - constants::damping * part.velocity[index];
            else 
                part.acceleration[index] -= constants::stiff_collisions * delt + constants::damping * part.velocity[index];;
        }
    }
}

void particlesMotion(particle& part, int index) {
    part.position[index] += part.boundary[index] * constants::delt_t + part.acceleration[index] * constants::delt_t * constants::delt_t;
    part.velocity[index] = part.boundary[index] + part.acceleration[index] * constants::delt_t / 2.0;
    part.boundary[index] += part.acceleration[index] * constants::delt_t;
}

void collideWithWall(particle& part, int index) {
    if (part.grid_positioning[index] == 0) {
        double dist = part.position[index] - constants::min[index];
        if (dist < 0) {
            part.position[index] = constants::min[index] - dist;
            part.velocity[index] *= -1.0;
            part.boundary[index] *= -1.0;
        }
    }
    else if (part.grid_positioning[index] == grid_size[index]-1) {
        double dist = constants::max[index] - part.position[index];
        if (dist < 0) {
            part.position[index] = constants::max[index] + dist;
            part.velocity[index] *= -1.0;
            part.boundary[index] *= -1.0;
        }
    }
}

void processStep() {
    repositionParticles();
    initializeDensityAndAcceleration();
    increaseVal(true);
    densityTransform();
    increaseVal(false);
    for (particle& part: particles) {
        for (int i = 0; i < 3; ++i) {
            updateAccelerationWithWall(part, i);
            particlesMotion(part, i);
            collideWithWall(part, i);
        }
    }
}

void writeFile(std::string outputFile) {
    std::ofstream fileWriter;
    fileWriter.open(outputFile, std::ios::binary);
    float x = ppm;
    write_binary_value(x, fileWriter);
    write_binary_value(np, fileWriter);
    for (particle p: particles) {
        for (int i = 0; i < 9; ++i) {
            float temp = 0.0;
            if (i < 3) temp = p.position[i%3];
            else if (i < 6) temp = p.boundary[i%3];
            else temp = p.velocity[i%3];
            write_binary_value(temp, fileWriter);
        }
    }
}


/*
    run with:
    g++ -o fluid fluid.cpp -std=c++20
    ./fluid **timestep** **inputfile** **outputfile**
    rm fluid.exe
*/
int main(int argc, const char* argv[]) {
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments: " << argc-1 << ".\n";
        return 1;
    }
    std::span const args_view{argv, static_cast<std::size_t>(argc)};
    std::vector<std::string> const arguments{args_view.begin() + 1, args_view.end()};

    int nts = parseInt(arguments[0]);
    parseInput(arguments[1]);
    for (unsigned int i = 0; i < nts; ++i) {
        std::cout << "step " << i << "\n";
        processStep();
    }
    writeFile(arguments[2]);
    /*
    for (particle p: particles) {
        std::cout << "particle " << p.id << ": " << p.density << " " << p.position[0] << " " << p.position[1] << " " << p.position[2] << "\n";
        std::cout << "velocity " << p.velocity[0] << " " << p.velocity[1] << " " << p.velocity[2] << "\n";
        std::cout << "boundary " << p.boundary[0] << " " << p.boundary[1] << " " << p.boundary[2] << "\n";
        std::cout << "acceleration  " << p.acceleration[0] << " " << p.acceleration[1] << " " << p.acceleration[2] << "\n";        
        std::cout << p.grid_positioning[0] << " " << p.grid_positioning[1] << " " << p.grid_positioning[2] << "\n";
    }*/
    
    binaryToText("large-5.fld", "large-5.txt");
    binaryToText(arguments[2], "final.txt");

    return 0;
}