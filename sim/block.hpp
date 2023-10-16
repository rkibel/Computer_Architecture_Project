#ifndef C8480EA9_0530_4381_8956_C2A5FD57C810
#define C8480EA9_0530_4381_8956_C2A5FD57C810

#include "particle.hpp"

struct block {
    std::vector<particle> particles;
    void push_back(particle part);
};

#endif /* C8480EA9_0530_4381_8956_C2A5FD57C810 */
