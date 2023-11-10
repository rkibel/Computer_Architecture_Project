#include "grid.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <cstdio>


class GridTest : public ::testing::Test {
protected:
    grid testGrid;
    std::string tempFileName;

    GridTest() : testGrid() {

        tempFileName = "temp_test_file.fld";
        std::ofstream out(tempFileName);
        out << "0.5 2 1.0 2.0 3.0 0.1 0.2 0.3 4.0 5.0 6.0 0.4 0.5 0.6";
        out.close();
    }
    // Think how can we use the constructor properly
    void SetUp() override {
        testGrid = parseInputFile(tempFileName);
    }

    void TearDown() override {

        std::remove(tempFileName.c_str());
    }
};

TEST_F(GridTest, GridInitialization) {
    // Check if grid is initialized with correct size
    //EXPECT_GT(testGrid.getSize(), 0);    how do we obtain the size?
}

TEST_F(GridTest, GridBoundaryConditions) {
    EXPECT_TRUE(testGrid.isOutsideGrid(-1, 0, 0));
    EXPECT_TRUE(testGrid.isOutsideGrid(0, -1, 0));
    EXPECT_TRUE(testGrid.isOutsideGrid(0, 0, -1));
}

TEST_F(GridTest, NeighborRetrieval) {
    auto neighbors = testGrid.getNeighbors(1, 1, 1);
    int expected_neighbor_count = 13;
    EXPECT_EQ(neighbors.size(), expected_neighbor_count);
}

TEST_F(GridTest, ParticleRepositioning) {
    testGrid.repositionAndInitialize();
    // Check the new position of particles
    // EXPECT_...(testGrid.CheckPosition()); // Do we have a method to check the position?
}

// Add more tests...


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}