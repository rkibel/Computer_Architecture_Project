// constants.hpp
#include <vector>

namespace constants {
  double const r      = 1.695;
  double const rho    = 1000;
  double const p_s    = 3.0;
  double const s_c    = 3E4;
  double const d_v    = 128.0;
  double const mu     = 0.4;
  double const d_p    = 2E-4;
  double const delt_t = 10E-3;
  const std::vector<double> acceleration{0.0, -9.8, 0.0};
  const std::vector<double> min{-0.065, -0.08, -0.065};
  const std::vector<double> max{0.065, .1, 0.065};
}  // namespace constants