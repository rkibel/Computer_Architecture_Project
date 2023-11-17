#include "../sim/progargs.hpp"
#include "utest_utility.hpp"

#include <cstdio>
#include <gtest/gtest.h>

class ProgargsTest : public testing::Test {
  public:
    void SetUp() override { }

    void TearDown() override { }

    params parameters;
    params correct_parameters;
    std::ifstream inFile;
    std::ofstream outFileTest;
    std::ofstream outFileCorrect;
};

TEST_F(ProgargsTest, checkArgNumber_Test) {
  EXPECT_EXIT(checkArgNumber(1), testing::ExitedWithCode(255),
              "Error: Invalid number of arguments: 0.\n");

  EXPECT_EXIT(checkArgNumber(3), testing::ExitedWithCode(255),
              "Error: Invalid number of arguments: 2.\n");

  EXPECT_EXIT(checkArgNumber(10), testing::ExitedWithCode(255),
              "Error: Invalid number of arguments: 9.\n");

  EXPECT_EXIT(
      {
        checkArgNumber(4);
        std::cerr << "function did not exit";
        exit(0);
      },
      testing::ExitedWithCode(0), "function did not exit");
}

TEST_F(ProgargsTest, parseInt_Test) {
  EXPECT_EXIT(parseInt("This won't work"), testing::ExitedWithCode(255),
              "Error: time steps must be numeric.\n");

  EXPECT_EXIT(parseInt(""), testing::ExitedWithCode(255), "Error: time steps must be numeric.\n");

  EXPECT_EXIT(parseInt("31415926535897942881273403249231843824914892318491258021753"),
              testing::ExitedWithCode(255), "Error: time steps must be numeric.\n");

  EXPECT_EXIT(parseInt("3.1415"), testing::ExitedWithCode(255),
              "Error: time steps must be numeric.\n");

  EXPECT_EXIT(parseInt("-1"), testing::ExitedWithCode(254),
              "Error: Invalid number of time steps.\n");

  EXPECT_EXIT(
      {
        parseInt("100");
        std::cerr << "function did not exit";
        exit(0);
      },
      testing::ExitedWithCode(0), "function did not exit");
}

TEST_F(ProgargsTest, parseInputFile_Test) {
  std::string const badInput1 = "Nothing";
  EXPECT_EXIT(parseInputFile(badInput1), testing::ExitedWithCode(253),
              "Error: Cannot open " + badInput1 + " for reading\n");
  EXPECT_EXIT(parseInputFile(""), testing::ExitedWithCode(253),
              "Error: Cannot open  for reading\n");
  std::string const badInput2 = "test-input/small-test-particles-mismatch.fld";
  EXPECT_EXIT(parseInputFile(badInput2), testing::ExitedWithCode(251),
              "Error: Number of particles mismatch. Header: 4750, Found: 4800.\n");
  std::string const badInput3 = "test-input/small-test-negative-particles.fld";
  EXPECT_EXIT(parseInputFile(badInput3), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: -1.\n");
  std::string const badInput4 = "test-input/small-test-zero-particles.fld";
  EXPECT_EXIT(parseInputFile(badInput4), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: 0.\n");

  std::string const goodInput = "reference-input/small.fld";
  EXPECT_EXIT(
      {
        parseInputFile(goodInput);
        std::cerr << "function did not exit";
        exit(0);
      },
      testing::ExitedWithCode(0), "function did not exit");
}

TEST_F(ProgargsTest, printGridInformation_Test) {
  std::string const inputFile = "reference-input/small.fld";
  EXPECT_EXIT(
      {
        printGridInformation(parseInputFile(inputFile).parameters);
        std::cerr << "function did not exit";
        exit(0);
      },
      testing::ExitedWithCode(0), "function did not exit");
  inFile.open(inputFile, std::ios::binary);
  // Will initialize basic values then also factors, which we are testing
  parameters.initialize(inFile);
}

TEST_F(ProgargsTest, writeFile_Test) {
    std::string const goodInput = "reference-input/small.fld";
    std::string const goodOutput = "output.fld";
    const float ppm = 204;
    const int numPart = 4800;
    EXPECT_EXIT( { writeFile(goodOutput, ppm, numPart, parseInputFile(goodInput).part_dict);
                     std::cerr << "function did not exit";
                     exit(0);
                 }, testing::ExitedWithCode(0), "function did not exit");
}