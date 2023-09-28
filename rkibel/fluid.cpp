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

std::vector<particle> parseInput(char* inputFile) {
    std::ifstream fileReader;
    fileReader.open(inputFile, std::ios::binary);
    if (!fileReader) {
        std::cerr << "Error: Cannot open " << inputFile << " for reading";
        exit(1);
    }
    std::vector<particle> particles;
    float ppm;
    int np;
    fileReader.read(reinterpret_cast<char*>(&ppm), sizeof(ppm));
    fileReader.read(reinterpret_cast<char*>(&np), sizeof(np));
    std::cout << ppm << " " << np;

    for (unsigned int i = 0; i < np; ++i) {
        particle p;
        fileReader.read(reinterpret_cast<char*>(&p.px), sizeof(p.px));
        fileReader.read(reinterpret_cast<char*>(&p.py), sizeof(p.py));
        fileReader.read(reinterpret_cast<char*>(&p.pz), sizeof(p.pz));
    }
    return particles;
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

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments: " << argc-1 << ".\n";
        return 1;
    }
    const std::string input_str = argv[1];
    int nts = parseInt(argv[1]);
    std::cout << nts << "\n";
    std::vector<particle> particles = parseInput(argv[2]);

    std::ofstream fout("output.txt"); //for testing purpose, to see if the string is a right copy
    std::ostringstream ostrm;

    return 0;
}