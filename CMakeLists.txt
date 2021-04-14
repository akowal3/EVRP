cmake_minimum_required(VERSION 3.16)
project(EVRP)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenMP REQUIRED)

# Include RoutingKit library 123
find_library(RK_LIB libroutingkit.so PATHS RoutingKit/lib)
include_directories(RoutingKit/include)
include_directories(include)

add_executable(EVRP src/main.cpp src/graph.cpp src/edge.cpp src/car.cpp include/car.h)

# Link external libraries to the executable
target_link_libraries(EVRP PUBLIC ${RK_LIB})
target_link_libraries(EVRP PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(EVRP PUBLIC Threads::Threads)
target_link_libraries(EVRP PUBLIC ZLIB::ZLIB)

