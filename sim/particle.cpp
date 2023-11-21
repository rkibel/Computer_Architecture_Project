#include "particle.hpp"

bool particle::operator==(particle const & other) const {
  double const density_tolerance = 1e-9;
  return id == other.id && grid_pos == other.grid_pos &&
         fabs(density - other.density) < density_tolerance &&
         std::vector<float>(position.begin(), position.end()) ==
             std::vector<float>(other.position.begin(), other.position.end()) &&
         std::vector<float>(boundary.begin(), boundary.end()) ==
             std::vector<float>(other.boundary.begin(), other.boundary.end()) &&
         std::vector<float>(velocity.begin(), velocity.end()) ==
             std::vector<float>(other.velocity.begin(), other.velocity.end()) &&
         std::vector<float>(acceleration.begin(), acceleration.end()) ==
             std::vector<float>(other.acceleration.begin(), other.acceleration.end());
}

std::ostream & operator<<(std::ostream & os, particle const & part) {
  os << "id: " << part.id << ", position: { ";
  for (auto const & pos : part.position) { os << static_cast<float>(pos) << " "; }
  os << "}, boundary: { ";
  for (auto const & bound : part.boundary) { os << static_cast<float>(bound) << " "; }
  os << "}, velocity: { ";
  for (auto const & vel : part.velocity) { os << static_cast<float>(vel) << " "; }
  os << "}, acceleration: { ";
  for (auto const & acc : part.acceleration) { os << static_cast<float>(acc) << " "; }
  os << "}, grid_pos: { ";
  for (auto const & gpos : part.grid_pos) { os << static_cast<float>(gpos) << " "; }
  os << "}, density: " << static_cast<float>(part.density) << "\n";
  return os;
}