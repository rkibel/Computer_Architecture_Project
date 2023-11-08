# Add to this list all files related to sim library
add_library(sim
        utility.cpp
        utility.hpp
        block.cpp
        block.hpp
        grid.cpp
        grid.hpp
        particle.hpp
        progargs.cpp
        progargs.hpp
)

# Use this line only if you have dependencies from stim to GSL
# target_link_libraries(sim PRIVATE Microsoft.GSL::GSL)