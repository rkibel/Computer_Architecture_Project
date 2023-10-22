#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include "params.hpp"
#include "grid.hpp"

#include <string>

void checkArgNumber(int argc);

int parseInt(std::string arg);

grid parseInputFile(std::string inputFile);

void writeFile(std::string outputFile, float ppm, int np, std::vector<particle> particles);

#endif // PROGARGS_HPP
