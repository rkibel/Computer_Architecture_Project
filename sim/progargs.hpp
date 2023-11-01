#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include "grid.hpp"
#include "params.hpp"
#include "utility.hpp"

#include <charconv>
#include <fstream>
#include <iostream>
#include <string>

void checkArgNumber(int argc);

int parseInt(std::string const & arg);

grid parseInputFile(std::string const & inputFile);

void printGridInformation(params const & parameters);

void writeFile(std::string const & outputFile, float ppm, int np,
               std::vector<particle> const & particles);

#endif  // PROGARGS_HPP
