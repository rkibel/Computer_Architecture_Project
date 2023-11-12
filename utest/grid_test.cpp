#include "grid.hpp"
#include <gtest/gtest.h>
#include <vector>
#include "../sim/progargs.hpp"

class GridTest : public ::testing::Test {
protected:
    grid testGrid;

    GridTest() : testGrid(parseInputFile("small.fld")) {
        // The member initializer list initializes testGrid with the result of parseInputFile.
    }

    void SetUp() override {
        // Read and parse "small.fld" into the testGrid instance.
        testGrid = parseInputFile("small.fld");
    }

    void TearDown() override {
        // No need to remove any temporary files here.
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
    EXPECT_FALSE(testGrid.isOutsideGrid(0, 0, 0));
    //EXPECT_TRUE(testGrid.isOutsideGrid(10, 9, 9)); // Test out of bounds positive index
    //EXPECT_FALSE(testGrid.isOutsideGrid(9, 9, 9)); // Test boundary condition at max size - 1
}

TEST_F(GridTest, NeighborRetrieval) {
    auto neighbors = testGrid.getNeighbors(1, 1, 1);
    const int expected_neighbor_count = 13;
    EXPECT_EQ(neighbors.size(), expected_neighbor_count);
}

TEST_F(GridTest, RepositionAndInitialize) {
    testGrid.repositionAndInitialize();
    // Verify the state of the grid after repositioning
    // You would check the positions of particles in the grid and other state variables
    // For example, check if particles are within the expected range
    // EXPECT_TRUE(/* condition to validate new positions */); // Replace with actual validation logic
}


TEST_F(GridTest, InitializeNeighborCombinations) {
    testGrid.initializeNeighborCombinations();
    // EXPECT_EQ(testGrid.grid_neighbor_combinations.size(), /* expected size */); // Replace with expected size
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}