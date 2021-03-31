cmake_minimum_required(VERSION 3.16)
project(EVRP)

set(CMAKE_CXX_STANDARD 20)

# Include RoutingKit library 123
find_library(RK_LIB libroutingkit.so PATHS RoutingKit/lib)
include_directories(RoutingKit/include)

add_executable(EVRP main.cpp)

# Link external libraries to the executable
target_link_libraries(EVRP ${RK_LIB})