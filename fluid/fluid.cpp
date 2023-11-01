#include "../sim/constants.hpp"
#include "../sim/grid.hpp"
#include "../sim/progargs.hpp"
#include "../sim/utility.hpp"

#include <iostream>
#include <span>
#include <vector>
#include <chrono>


int main(int argc, char const * argv[]) {
  checkArgNumber(argc);

  std::span const args_view {argv, static_cast<std::size_t>(argc)};
  std::vector<std::string> arguments {args_view.begin() + 1, args_view.end()};
  const int nts = parseInt(arguments[0]);
  grid particle_grid = parseInputFile(arguments[1]);

  using namespace std::chrono;
  using clk = high_resolution_clock;
  auto start = clk :: now();

  for (int i = 0; i < nts; ++i) { particle_grid.processStep(); }

  auto stop = clk :: now();
  auto diff = duration_cast<microseconds>(stop - start);
  std :: cout << "Time = " << diff .count() << "us\n";

  const auto read_ppm = static_cast<float>(particle_grid.parameters.ppm);
  const auto read_np = static_cast<int>(particle_grid.parameters.np);
  writeFile(arguments[2], read_ppm, read_np, particle_grid.part_dict);

  // remove this later, just for testing purposes
  binaryToText(arguments[2], "final.txt");

  return 0;
}