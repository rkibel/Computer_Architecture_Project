# Add to this list all files related to sim library
add_library(sim
    block.cpp
    block.hpp
    constants.hpp
    grid.cpp
    grid.hpp
    params.cpp
    params.hpp
    particle.hpp
    progargs.cpp
    progargs.hpp
    utility.cpp
    utility.hpp
)

# Use this line only if you have dependencies from stim to GSL
# target_link_libraries(sim PRIVATE Microsoft.GSL::GSL)