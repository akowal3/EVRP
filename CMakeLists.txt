cmake_minimum_required(VERSION 3.16)
project(EVRP)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(include)
include_directories(RoutingKit/include)

# Internal classes
#add_library(CAR SHARED src/car.cpp)
#add_library(EDGE SHARED src/edge.cpp)
#add_library(GRAPH SHARED src/graph.cpp)


# Main project executable
add_executable(EVRP src/main.cpp src/car.cpp src/edge.cpp src/graph.cpp src/node.cpp)
#target_link_libraries(EVRP PRIVATE CAR GRAPH EDGE)

# Link external libraries to the main executable
find_library(RK_LIB libroutingkit.so PATHS RoutingKit/lib)
target_link_libraries(EVRP PUBLIC ${RK_LIB} OpenMP::OpenMP_CXX Threads::Threads ZLIB::ZLIB)

# Catch2 unit tests
add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE testing/include)

add_executable(TESTS testing/test.cpp src/car.cpp src/edge.cpp src/graph.cpp testing/graph_test.cpp src/node.cpp)
target_link_libraries(TESTS Catch2 ${RK_LIB})

