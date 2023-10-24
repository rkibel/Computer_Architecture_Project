#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include "params.hpp"
#include "grid.hpp"
#include "utility.hpp"

#include <string>
#include <iostream>
#include <charconv>
#include <fstream>

void checkArgNumber(int argc);

int parseInt(std::string arg);

grid parseInputFile(std::string inputFile);

void writeFile(std::string outputFile, float ppm, int np, std::vector<particle> particles);

#endif // PROGARGS_HPP
