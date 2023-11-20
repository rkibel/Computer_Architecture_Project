#include "../sim/grid.hpp"
#include "../sim/progargs.hpp"

#include <gtest/gtest.h>

class GridTest : public testing::Test {
  public:
    void SetUp() override { }
    void TearDown() override { }
    
    int const steps = 5;

    template <typename T> requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
    static char * as_writable_buffer(T & value) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      return reinterpret_cast<char *>(&value);
    }

    static long read_long(std::istream & is) {
      long value{};
      is.read(as_writable_buffer(value), sizeof(value));
      return value;
    }

    static double read_double(std::istream & is) {
      double value{};
      is.read(as_writable_buffer(value), sizeof(value));
      return value;
    }

    static particle readParticle(std::istream & is, std::vector<int> & grid_pos) {
      particle part;
      part.id = static_cast<int>(read_long(is));
      int const three = 3;
      for (int i = 0; i < three; ++i) { part.position.push_back(read_double(is)); }
      for (int i = 0; i < three; ++i) { part.boundary.push_back(read_double(is)); }
      for (int i = 0; i < three; ++i) { part.velocity.push_back(read_double(is)); }
      part.density = read_double(is);
      for (int i = 0; i < three; ++i) { part.acceleration.push_back(read_double(is)); }
      part.grid_pos = grid_pos;
      return part;
    }

    static bool gridEqualsTrz(grid const & grid, std::string const & trz) {
      std::ifstream input;
      input.open(trz, std::ios::binary);
      if (!input) { 
        std::cerr << "Error: unable to open trz " << trz << " for reading."; 
        return false; 
      }
      std::vector<int> gsize = grid.parameters.grid_size;
      int const num_blocks = read_int(input);
      for (int i = 0; i < num_blocks; ++i) {
        std::vector<int> gpos = {i % gsize[0], static_cast<int>(i % (gsize[0] * gsize[1]) / gsize[0]),
          static_cast<int>(i / (gsize[0] * gsize[1])) };
        long const num_particles = read_long(input);
        for (long j = 0; j < num_particles; ++j) {
          particle const & ref = readParticle(input, gpos);
          particle const & org = grid.part_dict[ref.id];
          if (ref != org) {
            std::cerr << "Error: particles do not match.\nComputed:\n" << org << "Reference:\n" 
            << ref;
            return false;
          }
        }
      }
      return true;
    }

    // copy of for loop in grid.cpp processStep moved to named functions
    static void densityIncrement(grid & grid) {
      for (auto const & surround : grid.grid_neighbor_combinations) {
        for (size_t i = 0; i < surround.size(); ++i) {
          grid.updateBlock(surround[0], surround[i], true);
        }
      }
    }
    
    static void densityTransformation(grid & grid) {
      for (particle & part : grid.part_dict) {
        part.density = (part.density + grid.parameters.density_factors[1]) * 
        grid.parameters.density_factors[2];
      }
    }

    static void accelerationTransformation(grid & grid) {
      for (auto const & surround : grid.grid_neighbor_combinations) {
        for (size_t i = 0; i < surround.size(); ++i) {
          grid.updateBlock(surround[0], surround[i], false);
        }
      }
    }

    static void particleCollisions(grid & grid) {
      for (particle & part : grid.part_dict) { 
        grid.updateAccWithWall(part); 
      }
    }

    static void particleMotion(grid & grid) {
      for (particle & part : grid.part_dict) { 
        grid::particlesMotion(part);
      }
    }

    static void wallCollisions(grid & grid) {
      for (particle & part : grid.part_dict) {
        grid.collideWithWall(part);
      }
    }
};

TEST_F(GridTest, SmallTest) {
  grid small = parseInputFile("reference-input/small.fld");
  for (int i = 1; i < steps + 1; ++i) {
    std::string const stp = std::to_string(i);
    small.repositionAndInitialize();
    EXPECT_TRUE(gridEqualsTrz(small, "trace/small/repos-base-" + stp + ".trz"));
    EXPECT_TRUE(gridEqualsTrz(small, "trace/small/initacc-base-" + stp + ".trz"));
    densityIncrement(small);
    EXPECT_TRUE(gridEqualsTrz(small, "trace/small/densinc-base-" + stp + ".trz"));
    densityTransformation(small);
    EXPECT_TRUE(gridEqualsTrz(small, "trace/small/denstransf-base-" + stp + ".trz"));
    accelerationTransformation(small);
    EXPECT_TRUE(gridEqualsTrz(small, "trace/small/acctransf-base-" + stp + ".trz"));
    particleCollisions(small);
    EXPECT_TRUE(gridEqualsTrz(small, "trace/small/partcol-base-" + stp + ".trz"));
    particleMotion(small);
    EXPECT_TRUE(gridEqualsTrz(small, "trace/small/motion-base-" + stp + ".trz"));
    wallCollisions(small);
    EXPECT_TRUE(gridEqualsTrz(small, "trace/small/boundint-base-" + stp + ".trz"));
  }
}

TEST_F(GridTest, LargeTest) {
  grid large = parseInputFile("reference-input/large.fld");
  for (int i = 1; i < steps + 1; ++i) {
    std::string const stp = std::to_string(i);
    large.repositionAndInitialize();
    EXPECT_TRUE(gridEqualsTrz(large, "trace/large/repos-base-" + stp + ".trz"));
    EXPECT_TRUE(gridEqualsTrz(large, "trace/large/initacc-base-" + stp + ".trz"));
    densityIncrement(large);
    EXPECT_TRUE(gridEqualsTrz(large, "trace/large/densinc-base-" + stp + ".trz"));
    densityTransformation(large);
    EXPECT_TRUE(gridEqualsTrz(large, "trace/large/denstransf-base-" + stp + ".trz"));
    accelerationTransformation(large);
    EXPECT_TRUE(gridEqualsTrz(large, "trace/large/acctransf-base-" + stp + ".trz"));
    particleCollisions(large);
    EXPECT_TRUE(gridEqualsTrz(large, "trace/large/partcol-base-" + stp + ".trz"));
    particleMotion(large);
    EXPECT_TRUE(gridEqualsTrz(large, "trace/large/motion-base-" + stp + ".trz"));
    wallCollisions(large);
    EXPECT_TRUE(gridEqualsTrz(large, "trace/large/boundint-base-" + stp + ".trz"));
  }
}