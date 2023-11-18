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