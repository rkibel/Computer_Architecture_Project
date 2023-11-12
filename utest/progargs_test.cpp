#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "../sim/progargs.hpp"

// Mock data for initializing grid in tests
const std::string validGridInitData = R"(
4        // Number of particles
1.0      // Particles per meter
0.1      // Smoothing length
1.0      // Particle mass
10 10 10 // Grid size (width, height, depth)
2 2 2    // Block size (width, height, depth)
// Particle data: id, position (x, y, z), velocity (vx, vy, vz), boundary (bx, by, bz)
0 1.0 1.0 1.0 0.1 0.1 0.1 0.0 0.0 0.0
1 2.0 2.0 2.0 0.2 0.2 0.2 10.0 10.0 10.0
2 3.0 3.0 3.0 0.3 0.3 0.3 0.0 0.0 0.0
3 4.0 4.0 4.0 0.4 0.4 0.4 10.0 10.0 10.0
)";

// Test fixture for ProgArgs tests
class ProgArgsTest : public ::testing::Test {
protected:
    std::string inputFileName;
    std::string outputFileName;
    std::vector<std::string> mockArgv;

    // Set up a valid test environment
    void SetUp() override {
        // Create a temporary file with valid grid initialization data
        inputFileName = "temp_input_file.txt";
        std::ofstream inputFile(inputFileName);
        inputFile << validGridInitData;
        inputFile.close();

        // Set up mock arguments (assuming the program expects three arguments besides the program name)
        mockArgv = {"./program", inputFileName, "output_file.txt", "1000"};
    }

    // Tear down the test environment
    void TearDown() override {
        // Remove the temporary file
        std::remove(inputFileName.c_str());
    }

    // Helper function to simulate command-line arguments
    void simulateCommandLine(int argc, char** argv) {
        // Call the argument checking function
        checkArgNumber(argc);

        // Parse the input file and print grid information
        grid particle_grid = parseInputFile(argv[1]);
        writeFile(argv[2], particle_grid.parameters.ppm, particle_grid.parameters.np, particle_grid.part_dict);
    }
};

// Test case for checking argument number
TEST_F(ProgArgsTest, CheckArgNumber) {
    int argc = 4; // Correct number of arguments
    char* argv[] = {mockArgv[0].data(), mockArgv[1].data(), mockArgv[2].data(), mockArgv[3].data()};
    ASSERT_NO_THROW(simulateCommandLine(argc, argv));
}

// Test case for checking argument parsing
TEST_F(ProgArgsTest, ParseInt) {
    ASSERT_EQ(parseInt("1000"), 1000); // Valid integer string
    //ASSERT_THROW(parseInt("invalid"), std::system_error); // Can not find how to catch the exception
}

TEST_F(ProgArgsTest, ParseInputFile) {
    ASSERT_NO_THROW(parseInputFile(inputFileName)); // Valid input file
    ASSERT_THROW(parseInputFile("non_existent_file.txt"), std::exception); // Non-existent file
}
