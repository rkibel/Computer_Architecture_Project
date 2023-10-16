#include <span>
#include <iostream>
#include <vector>
#include "../sim/progargs.hpp"

int main(int argc, const char* argv[]) {
    checkArgNumber(argc);
    std::span const args_view{argv, static_cast<std::size_t>(argc)};
    std::vector<std::string> const arguments{args_view.begin() + 1, args_view.end()};

    int nts = parseInt(arguments[0]);
    parseInputFile(arguments[1]);
    for (unsigned int i = 0; i < nts; ++i) {
        std::cout << "step " << i << "\n";
        //processStep();
    }
    //writeFile(arguments[2]);

    return 0;
}