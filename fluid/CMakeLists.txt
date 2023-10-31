add_executable(fluid fluid.cpp)

target_include_directories(fluid PRIVATE ${CMAKE_SOURCE_DIR}/../sim)

target_link_libraries(fluid sim)