#include "block.hpp"

void block::push_back(int part) {
    particles.push_back(part);
}

int block::size() {
    return particles.size();
}

int block::getBlockId(int index) {
    return particles[index];
}