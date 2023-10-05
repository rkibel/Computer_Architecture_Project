#include <iostream>
#include <fstream>
#include <sstream>

void binaryToText(const char* inputFile, const char* outputFile) {
    std::ifstream inFile;
    inFile.open(inputFile, std::ios::binary);

    std::ofstream outFile;
    outFile.open(outputFile);

    float ppm = 0.0;
    int np = 0;
    inFile.read(reinterpret_cast<char*>(&ppm), sizeof(ppm));
    inFile.read(reinterpret_cast<char*>(&np), sizeof(np));
    outFile << ppm << " " << np << "\n";

    while (!inFile.eof()) {
        float px, py, pz, hvx, hvy, hvz, vx, vy, vz;
        inFile.read(reinterpret_cast<char*>(&px), sizeof(float));
        inFile.read(reinterpret_cast<char*>(&py), sizeof(float));
        inFile.read(reinterpret_cast<char*>(&pz), sizeof(float));
        inFile.read(reinterpret_cast<char*>(&hvx), sizeof(float));
        inFile.read(reinterpret_cast<char*>(&hvy), sizeof(float));
        inFile.read(reinterpret_cast<char*>(&hvz), sizeof(float));
        inFile.read(reinterpret_cast<char*>(&vx), sizeof(float));
        inFile.read(reinterpret_cast<char*>(&vy), sizeof(float));
        inFile.read(reinterpret_cast<char*>(&vz), sizeof(float));
        outFile << px << " " << py << " " << pz << " " << hvx << " " << hvy << " " << hvz << " " << vx << " " << vy << " " << vz << "\n";
    }
}