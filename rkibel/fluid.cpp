#include <iostream>
#include <string>
#include <charconv>

int parseInt(char* arg) {
    const std::string input_str = arg;
    int res;
    auto result = std::from_chars(input_str.data(), input_str.data() + input_str.size(), res);
    if (result.ec != std::errc()) {
        std::cerr << "Error: time steps must be numeric.\n";
        exit(1);
    }
    if (res < 0) {
        std::cerr << "Error: Invalid number of time steps.\n";
        exit(1);
    }
    return res;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments: " << argc-1 << ".\n";
        return 1;
    }
    const std::string input_str = argv[1];
    int nts = parseInt(argv[1]);
    std::cout << nts;

    return 0;
}