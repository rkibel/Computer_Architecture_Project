# Add to this list all files related to sim library
add_library(sim
        utility.hpp
        utility.cpp
        block.cpp
        block.hpp
        constants.hpp
        constants.cpp
        grid.cpp
        grid.hpp
        params.cpp
        params.hpp
        particle.hpp
        progargs.cpp
        progargs.hpp
)

# Use this line only if you have dependencies from stim to GSL
# target_link_libraries(sim PRIVATE Microsoft.GSL::GSL)