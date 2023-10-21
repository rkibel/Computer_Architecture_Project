#ifndef B5509B7D_AC86_4A5B_B216_7F64F52A5223
#define B5509B7D_AC86_4A5B_B216_7F64F52A5223

#include "params.hpp"
#include "grid.hpp"

#include <string>

void checkArgNumber(int argc);

int parseInt(std::string arg);

grid parseInputFile(std::string inputFile);

void writeFile(std::string outputFile, float ppm, int np, std::vector<particle> particles);

#endif /* B5509B7D_AC86_4A5B_B216_7F64F52A5223 */
