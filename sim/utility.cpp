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

bool compare_binary_files(std::string const & filename1, std::string const & filename2) {
  std::ifstream file1(filename1, std::ifstream::ate | std::ifstream::binary);
  std::ifstream file2(filename2, std::ifstream::ate | std::ifstream::binary);
  std::ifstream::pos_type const fileSize = file1.tellg();
  if (fileSize != file2.tellg()) { return false; }
  file1.seekg(0);
  file2.seekg(0);
  std::istreambuf_iterator<char> const begin1(file1);
  std::istreambuf_iterator<char> const begin2(file2);
  return std::equal(begin1, std::istreambuf_iterator<char>(), begin2);
}

// TODO: DELETE LATER
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

// TODO: DELETE LATER
void textToBinary(std::string const & inputFile, std::string const & outputFile) {
  std::ifstream inFile;
  inFile.open(inputFile, std::ios::in);
  float ppm  = 0.0;
  int n_part = 0;
  inFile >> ppm >> n_part;
  std::ofstream outFile;
  outFile.open(outputFile, std::ios::binary);
  if (!outFile.is_open()) {
    std::cerr << "Error opening file.\n";
    exit(1);
  }
  write_float(ppm, outFile);
  write_int(n_part, outFile);
  while (!inFile.eof()) {
    int const nine = 9;
    for (int i = 0; i < nine; i++) {
      float temp = 0.0;
      inFile >> temp;
      write_float(temp, outFile);
    }
  }
}
