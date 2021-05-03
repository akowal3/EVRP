cmake_minimum_required(VERSION 3.16)
project(EVRP)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(include)
include_directories(RoutingKit/include)

### LIBRARIES & COMMON FILES ###
# Combine common classes into single library
add_library(COMMON src/car.cpp src/edge.cpp src/graph.cpp src/node.cpp)

# Link external libraries to the main executable
find_library(RK_LIB libroutingkit.so PATHS RoutingKit/lib) # RoutingKit library
add_library(Catch2 INTERFACE) # Catch2 unit tests library
target_include_directories(Catch2 INTERFACE testing/include)


### EXECUTABLES ###
# Main project executable
add_executable(EVRP src/main.cpp)
target_link_libraries(EVRP PUBLIC ${RK_LIB} OpenMP::OpenMP_CXX Threads::Threads ZLIB::ZLIB COMMON)

# Test executable using Catch2
# Longer test cases get split into separate files. Include those files to run tests
add_library(TESTCASES INTERFACE)
#target_link_libraries(TESTCASES Catch2)
target_include_directories(TESTCASES INTERFACE testing/testcases)


add_executable(TESTS testing/test.cpp testing/graph_test.cpp testing/car_test.cpp)
target_link_libraries(TESTS Catch2 ${RK_LIB} COMMON TESTCASES)


