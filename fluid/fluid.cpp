#include <span>
#include <iostream>
#include <vector>
#include "../sim/progargs.hpp"
#include "../sim/utility.cpp"
#include "../sim/grid.hpp"


int main(int argc, const char* argv[]) {
    checkArgNumber(argc);
    std::span const args_view{argv, static_cast<std::size_t>(argc)};
    std::vector<std::string> const arguments{args_view.begin() + 1, args_view.end()};
    const int nts = parseInt(arguments[0]);
    grid particle_grid = parseInputFile(arguments[1]);
    for (int i = 0; i < nts; ++i) {
        std::cout << "step " << i << "\n";
        particle_grid.processStep();
    }
    writeFile(arguments[2], particle_grid.parameters.ppm, particle_grid.parameters.np, particle_grid.part_dict);
    //binaryToText("small.fld", "small1.txt");
    //binaryToText("small-1.fld", "small-1.txt");
    //binaryToText("small-5.fld", "small-5.txt");
    binaryToText(arguments[2], "final1.txt");

    return 0;
}