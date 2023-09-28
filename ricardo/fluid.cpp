#include <iostream>

// To compile and run from the terminal use:
// g++ -o fluid.exe fluid.cpp
// ./fluid.exe "param1" "param2" ...

int main(int argc, char *argv[]) {
    if (argc != 4){
        std::cout << "Error: Invalid number of arguments: " << argc-1 << std::endl;
        return -1;
    }
    std::cout << "Hello, World!" << std::endl;
    std::cout << argc << std::endl;
    return 0;
}
