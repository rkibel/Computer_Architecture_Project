#include "../sim/constants.hpp"
#include "../sim/grid.hpp"
#include "../sim/progargs.hpp"
#include "../sim/utility.cpp"

#include <iostream>
#include <span>
#include <vector>

int main(int argc, char const * argv[]) {
  /*
  checkArgNumber(argc);
  std::span const args_view{argv, static_cast<std::size_t>(argc)};
  std::vector<std::string> const arguments{args_view.begin() + 1, args_view.end()};
  const int nts = parseInt(arguments[0]);
  grid particle_grid = parseInputFile(arguments[1]);
  for (int i = 0; i < nts; ++i) {
      std::cout << "step " << i << "\n";
      particle_grid.processStep();
  }
  writeFile(arguments[2], static_cast<float>(particle_grid.parameters.ppm),
  static_cast<int>(particle_grid.parameters.np), particle_grid.part_dict);
  */
  // Small comment, adding
  // binaryToText("small.fld", "small.txt");
  // binaryToText("reference-output/small-1.fld", "reference-output/small-1.txt");
  // binaryToText("reference-output/small-2.fld", "reference-output/small-2.txt");
  // binaryToText("reference-output/small-5.fld", "reference-output/small-5.txt");
  // binaryToText("reference-output/small-10.fld", "reference-output/small-10.txt");

  // binaryToText(arguments[2], "final-5.txt");
  constants::acceleration = {0.0, -9.8, 0.0};
  constants::min          = {-0.065, -0.08, -0.065};
  constants::max          = {0.065, 0.1, 0.065};
  return 0;
}