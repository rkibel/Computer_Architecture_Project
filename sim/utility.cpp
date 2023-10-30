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

/*void binaryToText(const std::string& inputFile, const std::string& outputFile) {
    std::cout << "Hola";
    std::ifstream inFile(inputFile, std::ios::binary);
    inFile.open(inputFile, std::ios::binary);
    std::ofstream outFile(outputFile);
    outFile.open(outputFile);

    const float ppm = read_float(inFile);
    const int n_part = read_int(inFile);
    outFile << ppm << " " << n_part << "\n";
    while (!inFile.eof()) {
        const float pos_x = read_float(inFile);
        const float pos_y = read_float(inFile);
        const float pos_z = read_float(inFile);
        const float hvx = read_float(inFile);
        const float hvy = read_float(inFile);
        const float hvz = read_float(inFile);
        const float vel_x = read_float(inFile);
        const float vel_y = read_float(inFile);
        const float vel_z = read_float(inFile);
        outFile << pos_x << " " << pos_y << " " << pos_z << " " << hvx << " " << hvy << " " << hvz << " " << vel_x << " " << vel_y << " " << vel_z << "\n";
    }
}*/
void binaryToText(std::string inputFile, std::string outputFile) {
    std::ifstream inFile;
    inFile.open(inputFile, std::ios::binary);

    std::ofstream outFile;
    outFile.open(outputFile);

    const float ppm = read_float(inFile);
    const int n_part = read_int(inFile);
    outFile << ppm << " " << n_part << "\n";

    while (!inFile.eof()) {
        const float pos_x = read_float(inFile);
        const float pos_y = read_float(inFile);
        const float pos_z = read_float(inFile);
        const float hvx = read_float(inFile);
        const float hvy = read_float(inFile);
        const float hvz = read_float(inFile);
        const float vel_x = read_float(inFile);
        const float vel_y = read_float(inFile);
        const float vel_z = read_float(inFile);
        outFile << pos_x << " " << pos_y << " " << pos_z << " " << hvx << " " << hvy << " " << hvz << " " << vel_x << " " << vel_y << " " << vel_z << "\n";
    }
}