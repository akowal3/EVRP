cmake_minimum_required(VERSION 3.16)
project(EVRP)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(EVRP src/main.cpp src/graph.cpp src/edge.cpp src/car.cpp)

target_include_directories(EVRP PRIVATE include)
# Include RoutingKit library
target_include_directories(EVRP PRIVATE RoutingKit/include)

# Link external libraries to the executable
find_library(RK_LIB libroutingkit.so PATHS RoutingKit/lib)
target_link_libraries(EVRP PUBLIC ${RK_LIB})
target_link_libraries(EVRP PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(EVRP PUBLIC Threads::Threads)
target_link_libraries(EVRP PUBLIC ZLIB::ZLIB)

# Catch2 unit tests
add_executable(TESTS src/car.cpp testing/car_test.cpp testing/test.cpp)
target_include_directories(TESTS PRIVATE testing/include)
target_include_directories(TESTS PRIVATE include)

