#include "../sim/constants.hpp"
#include "../sim/grid.hpp"
#include "../sim/progargs.hpp"
#include "../sim/utility.hpp"

#include <iostream>
#include <span>
#include <vector>

int main(int argc, char const * argv[]) {
  checkArgNumber(argc);
  std::span const args_view{argv, static_cast<std::size_t>(argc)};
  std::vector<std::string> arguments{args_view.begin() + 1, args_view.end()};
  int const nts = parseInt(arguments[0]);
  std::cout << "Parsed int"
            << "\n";
  grid particle_grid = parseInputFile(arguments[1]);
  std::cout << "Created particle grid"
            << "\n";
  for (int i = 0; i < nts; ++i) {
    std::cout << "step " << i + 1 << "\n";
    particle_grid.processStep();
  }
  writeFile(arguments[2], static_cast<float>(particle_grid.parameters.ppm),
            static_cast<int>(particle_grid.parameters.np), particle_grid.part_dict);
  // Small comment, adding
  binaryToText("small.fld", "small-initial.txt");
  binaryToText("reference-output/small-1.fld", "reference-output/small-1.txt");
  binaryToText("reference-output/small-2.fld", "reference-output/small-2.txt");
  binaryToText("reference-output/small-5.fld", "reference-output/small-5.txt");
  binaryToText("reference-output/small-10.fld", "reference-output/small-10.txt");
  binaryToText(arguments[2], "output.txt");
  return 0;
}