#include "utility.hpp"

template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char * as_writable_buffer(T & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char *>(&value);
}

float read_float(std::istream& is) {
  float value{};
  is.read(as_writable_buffer(value), sizeof(value));
  return value;
}

int read_int(std::istream& is) {
  int value{};
  is.read(as_writable_buffer(value), sizeof(value));
  return value;
}

template <typename T>
  requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char const * as_buffer(T const & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char const *>(&value);
}

void write_float(float value, std::ostream& os) {
  os.write(as_buffer(value), sizeof(value));
}

void write_int(int value, std::ostream& os) {
  os.write(as_buffer(value), sizeof(value));
}

void binaryToText(std::string inputFile, std::string outputFile) {
    std::ifstream inFile;
    inFile.open(inputFile, std::ios::binary);

    std::ofstream outFile;
    outFile.open(outputFile);

    float ppm = read_float(inFile);
    int np = read_int(inFile);
    outFile << ppm << " " << np << "\n";

    while (!inFile.eof()) {
        float px = read_float(inFile);
        float py = read_float(inFile);
        float pz = read_float(inFile);
        float hvx = read_float(inFile);
        float hvy = read_float(inFile);
        float hvz = read_float(inFile);
        float vx = read_float(inFile);
        float vy = read_float(inFile);
        float vz = read_float(inFile);
        outFile << px << " " << py << " " << pz << " " << hvx << " " << hvy << " " << hvz << " " << vx << " " << vy << " " << vz << "\n";
    }
}