#include "../sim/grid.hpp"
#include "../sim/progargs.hpp"

#include <gtest/gtest.h>
#include <cstdio>
#include <span>

class FTest : public testing::Test {
  public:
    void SetUp() override { }
    void TearDown() override { }
    
    int const step_count = 5;

    static int main_copy(int const argc, std::vector<std::string> & argv) {
      checkArgNumber(argc);
      argv.erase(argv.begin());
      int const nts      = parseInt(argv[0]);
      grid particle_grid = parseInputFile(argv[1]);
      for (int i = 0; i < nts; ++i) { particle_grid.processStep(); }
      auto const read_ppm = static_cast<float>(particle_grid.parameters.ppm);
      auto const read_np  = static_cast<int>(particle_grid.parameters.np);
      writeFile("ftest/" + argv[2], read_ppm, read_np, particle_grid.part_dict);
      return 0;
    }
};

TEST_F(FTest, TestSmall) {
  for (int i = 1; i < step_count+1; ++i) {
    std::string const file_name   = "small-" + std::to_string(i) + ".fld";
    std::vector<std::string> argv = {"run.sh", std::to_string(i), "in/small.fld",
                                     file_name};
    int const argc                = static_cast<int>(argv.size());
    main_copy(argc, argv);
    EXPECT_TRUE(compare_binary_files("ftest/" + file_name, "out/" + file_name));
    EXPECT_EQ(remove(("ftest/" + file_name).c_str()), 0);
  }
}

TEST_F(FTest, TestLarge) {
  for (int i = 1; i < step_count+1; ++i) {
    std::string const file_name   = "large-" + std::to_string(i) + ".fld";
    std::vector<std::string> argv = {"run.sh", std::to_string(i), "in/large.fld",
                                     file_name};
    int const argc                = static_cast<int>(argv.size());
    main_copy(argc, argv);
    EXPECT_TRUE(compare_binary_files("ftest/" + file_name, "out/" + file_name));
    EXPECT_EQ(remove(("ftest/" + file_name).c_str()), 0);
  }
}

TEST_F(FTest, TestInvalidNumberArguments) {
  std::vector<std::string> argv1 = {"run.sh"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv1.size()), argv1), testing::ExitedWithCode(255),
              "Error: Invalid number of arguments: 0.\n");
  std::vector<std::string> argv2 = {"run.sh", "4"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv2.size()), argv2), testing::ExitedWithCode(255),
              "Error: Invalid number of arguments: 1.\n");
  std::vector<std::string> argv3 = {"run.sh", "2000", "init.fld"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv3.size()), argv3), testing::ExitedWithCode(255),
              "Error: Invalid number of arguments: 2.\n");
  std::vector<std::string> argv4 = {"run.sh", "2000", "init.fld", "final.fld", "45"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv4.size()), argv4), testing::ExitedWithCode(255),
              "Error: Invalid number of arguments: 4.\n");
}

TEST_F(FTest, TestInvalidArgumentType) {
  std::vector<std::string> argv1 = {"run.sh", "hello", "init.fld", "final.fld"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv1.size()), argv1), testing::ExitedWithCode(255),
              "Error: time steps must be numeric.\n");
  std::vector<std::string> argv2 = {"run.sh", "3.1415", "init.fld", "final.fld"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv2.size()), argv2), testing::ExitedWithCode(255),
              "Error: time steps must be numeric.\n");
  std::vector<std::string> argv3 = {"run.sh", "-3", "init.fld", "final.fld"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv3.size()), argv3), testing::ExitedWithCode(254),
              "Error: Invalid number of time steps.\n");
  std::vector<std::string> argv4 = {"run.sh", "1", "in/init.fld", "final.fld"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv4.size()), argv4), testing::ExitedWithCode(253),
              "Error: Cannot open in/init.fld for reading\n");
  std::vector<std::string> argv5 = {"run.sh", "1", "in/small.fld", "empty_dir/"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv5.size()), argv5), testing::ExitedWithCode(252),
              "Error: Cannot open ftest/empty_dir/ for writing\n");
}

TEST_F(FTest, TestInputDiscrepancies) {
  std::vector<std::string> argv1 = {"run.sh", "2000", "test-in/small-test-zero-particles.fld",
                                    "final.fld"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv1.size()), argv1), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: 0.\n");
  std::vector<std::string> argv2 = {"run.sh", "2000",
                                    "test-in/small-test-negative-particles.fld", "final.fld"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv2.size()), argv2), testing::ExitedWithCode(251),
              "Error: Invalid number of particles: -1.\n");
  std::vector<std::string> argv3 = {"run.sh", "2000",
                                    "test-in/small-test-particles-mismatch.fld", "final.fld"};
  EXPECT_EXIT(main_copy(static_cast<int>(argv3.size()), argv3), testing::ExitedWithCode(251),
              "Number of particles mismatch. Header: 4750, Found: 4800.");
}