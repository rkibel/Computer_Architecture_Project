#ifndef C8480EA9_0530_4381_8956_C2A5FD57C810
#define C8480EA9_0530_4381_8956_C2A5FD57C810

#include <vector>

struct block {
    std::vector<int> particles;
    void push_back(int part);
    int size();
    int getBlockId(int index);
};

#endif /* C8480EA9_0530_4381_8956_C2A5FD57C810 */
