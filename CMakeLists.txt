cmake_minimum_required(VERSION 3.16)
project(EVRP)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(include)
include_directories(dependencies/RoutingKit/include)

### LIBRARIES & COMMON FILES ###
# Combine common classes into single library
add_library(UTILS src/algorithm/utils/utils.cpp)

add_library(COMMON src/algorithm/common/car.cpp src/algorithm/common/edge.cpp src/algorithm/common/node.cpp src/algorithm/common/charger_profile.cpp)
target_link_libraries(COMMON PUBLIC UTILS)

add_library(GRAPH src/algorithm/graph/graph.cpp)
target_link_libraries(GRAPH PUBLIC COMMON)

add_library(ROUTER src/algorithm/router/router.cpp src/algorithm/router/label.cpp)
target_link_libraries(ROUTER PUBLIC COMMON)

# Link external libraries to the main executable

## RoutingKit build ##
add_custom_target(
        build_RK_LIB
        COMMAND $(MAKE) --silent
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RoutingKit/
        COMMENT "Build RoutingKit library"
)
add_library(RK_LIB SHARED IMPORTED)
set_target_properties(RK_LIB PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RoutingKit/lib/libroutingkit.so)
add_dependencies(RK_LIB build_RK_LIB)

add_library(Catch2 INTERFACE) # Catch2 unit tests library
target_include_directories(Catch2 INTERFACE testing/include)

### EXECUTABLES ###
# Main project executable
add_executable(EVRP src/algorithm/main.cpp)
target_link_libraries(EVRP PUBLIC RK_LIB COMMON ROUTER)

# Test executable using Catch2
# Longer test cases get split into separate files. Include those files to run tests
add_library(TESTCASES testing/testcases/graph_cases.cpp)
#target_link_libraries(TESTCASES Catch2)
target_include_directories(TESTCASES PUBLIC testing/include)
target_include_directories(TESTCASES PUBLIC testing/testcases)

add_executable(TESTS testing/test.cpp testing/graph_test.cpp testing/car_test.cpp testing/router_test.cpp testing/charger_test.cpp)

target_link_libraries(TESTS RK_LIB TESTCASES COMMON ROUTER GRAPH UTILS Catch2)

#add_dependencies(TESTS build_RK_LIB)

#target_include_directories(TESTS PUBLIC testing/testcases)


