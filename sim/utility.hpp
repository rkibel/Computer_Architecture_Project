#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <type_traits>
#include <iostream>
#include <fstream>

template <typename T>
  requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char* as_writable_buffer(T& value);

float read_float(std::istream& is);
int read_int(std::istream& is);

template <typename T>
  requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char const * as_buffer(T const & value);

void write_float(float value, std::ostream& os);
void write_int(int value, std::ostream& os);

void binaryToText(const std::string& inputFile, const std::string& outputFile);

#endif // UTILITY_HPP
