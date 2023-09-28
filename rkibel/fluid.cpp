#include <iostream>
#include <vector>
#include <charconv>
#include <fstream>
#include <sstream>

struct particle {
    float px;
    float py;
    float pz;
    float hvx;
    float hvy;
    float hvz;
    float vx;
    float vy;
    float vz;
};

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

    for (unsigned int i = 0; i < np; ++i) {
        particle p;
        fileReader.read(reinterpret_cast<char*>(&p), sizeof(particle));
        particles.push_back(p);
    }
    return particles;
}

void testOutput(char* outputFile) {
    return;
}


/*
run with
g++ -o fluid fluid.cpp
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
    }
    testOutput(argv[3]);

    return 0;
}