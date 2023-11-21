#include "../sim/progargs.hpp"
#include <gtest/gtest.h>

class ProgargsTest : public testing::Test {
  public:
    void SetUp() override { }
    void TearDown() override { }
};

TEST_F(ProgargsTest, checkArgNumberTest) {
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

TEST_F(ProgargsTest, parseIntTest) {
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

TEST_F(ProgargsTest, parseInputFileTest) {
  std::string const badInput1 = "invalid_file";
  EXPECT_EXIT(parseInputFile(badInput1), testing::ExitedWithCode(253),
              "Error: Cannot open " + badInput1 + " for reading\n");
  EXPECT_EXIT(parseInputFile(""), testing::ExitedWithCode(253),
              "Error: Cannot open  for reading\n");
  std::string const badInput2 = "test-in/small-test-particles-mismatch.fld";
  EXPECT_EXIT(parseInputFile(badInput2), testing::ExitedWithCode(251),
              "Error: Number of particles mismatch. Header: 4750, Found: 4800.\n");
  std::string const badInput3 = "test-in/small-test-negative-particles.fld";
  EXPECT_EXIT(parseInputFile(badInput3), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: -1.\n");
  std::string const badInput4 = "test-in/small-test-zero-particles.fld";
  EXPECT_EXIT(parseInputFile(badInput4), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: 0.\n");
  std::string const goodInput = "in/small.fld";
  testing::internal::CaptureStdout();
  EXPECT_EXIT( { parseInputFile(goodInput); exit(0); }, testing::ExitedWithCode(0), "");
  std::string const expectedOutput = "Number of particles: 4800\nParticles per meter: 204\n"
   "Smoothing length: 0.00830882\nParticle mass: 0.00011779\nGrid size: 15 x 21 x 15\n"
   "Number of blocks: 4725\nBlock size: 0.00866667 x 0.00857143 x 0.00866667\n";
  EXPECT_EQ(testing::internal::GetCapturedStdout(), expectedOutput);
}

TEST_F(ProgargsTest, writeFileTest) {
    std::string const goodInput = "in/small.fld";
    std::string const goodOutput = "utest/output.fld";
    std::string const badOutput = "directory/";
    float const ppm = 204;
    int const numPart = 4800;
    std::vector<particle> const particles = parseInputFile(goodInput).part_dict;
    writeFile(goodOutput, ppm, numPart, particles);
    EXPECT_TRUE(compare_binary_files(goodOutput, goodInput));
    EXPECT_EQ(remove((goodOutput).c_str()), 0);
    EXPECT_EXIT(writeFile(badOutput, ppm, numPart, particles), testing::ExitedWithCode(252),
                "Error: Cannot open " + badOutput + " for writing\n");
}