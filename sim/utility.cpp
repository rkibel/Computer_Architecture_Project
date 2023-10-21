#include <iostream>
#include <fstream>

/*template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char * as_writable_buffer(T & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char *>(&value);
}

template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
T read_binary_value(std::istream & is) {
  T value{};
  is.read(as_writable_buffer(value), sizeof(value));
  return value;
}

template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char const * as_buffer(T const & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char const *>(&value);
}

template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
void write_binary_value(T value, std::ostream & os) {
  os.write(as_buffer(value), sizeof(value));
}*/

void binaryToText(std::string inputFile, std::string outputFile) {
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