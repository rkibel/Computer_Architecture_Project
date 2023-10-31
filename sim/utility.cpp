#include "utility.hpp"

template <typename T>
  requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char * as_writable_buffer(T & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char *>(&value);
}

float read_float(std::istream & is) {
  float value{};
  is.read(as_writable_buffer(value), sizeof(value));
  return value;
}

int read_int(std::istream & is) {
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

void write_float(float value, std::ostream & os) {
  os.write(as_buffer(value), sizeof(value));
}

void write_int(int value, std::ostream & os) {
  os.write(as_buffer(value), sizeof(value));
}

void binaryToText(std::string const & inputFile, std::string const & outputFile) {
  std::ifstream inFile;
  inFile.open(inputFile, std::ios::binary);

  std::ofstream outFile;
  outFile.open(outputFile);

  float const ppm  = read_float(inFile);
  int const n_part = read_int(inFile);
  outFile << ppm << " " << n_part << "\n";

  while (!inFile.eof()) {
    float const pos_x = read_float(inFile);
    float const pos_y = read_float(inFile);
    float const pos_z = read_float(inFile);
    float const hvx   = read_float(inFile);
    float const hvy   = read_float(inFile);
    float const hvz   = read_float(inFile);
    float const vel_x = read_float(inFile);
    float const vel_y = read_float(inFile);
    float const vel_z = read_float(inFile);
    outFile << pos_x << " " << pos_y << " " << pos_z << " " << hvx << " " << hvy << " " << hvz
            << " " << vel_x << " " << vel_y << " " << vel_z << "\n";
  }
}