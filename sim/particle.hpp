#ifndef E137B33D_793D_413F_9B5B_E2057DAF9EE5
#define E137B33D_793D_413F_9B5B_E2057DAF9EE5

#include <vector>

struct particle {
    int id;
    std::vector<double> position;
    std::vector<double> boundary;
    std::vector<double> velocity;
    std::vector<double> acceleration;
    double density;
};

#endif /* E137B33D_793D_413F_9B5B_E2057DAF9EE5 */
