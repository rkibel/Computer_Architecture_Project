#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include "grid.hpp"
#include "utility.hpp"

#include <charconv>
#include <fstream>
#include <iostream>
#include <string>

void checkArgNumber(int argc);

int parseInt(std::string const & arg);

grid parseInputFile(std::string const & inputFile);

void printGridInformation(grid const & grid);

void writeFile(std::string const & outputFile, grid const & grid);

#endif  // PROGARGS_HPP
