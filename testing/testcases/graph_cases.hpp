#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by akowal3 on 01/05/2021.
//

#ifndef EVRP_GRAPH_CASES_HPP
#define EVRP_GRAPH_CASES_HPP

#include <edge.h>

#include <testcase.hpp>
#include <vector>

std::vector<Testcase> GraphCases = {
    Testcase("Graph with single edge",
             {
                     BuildingEdge(0, 1, 100, 80),
             },
             2),
    Testcase("Graph with single bidirectional edge",
             {
                     BuildingEdge(0, 1, 100, 80),
                     BuildingEdge(1, 0, 100, 80),
             },
             2),
    Testcase("Graph with 2 same edges",
             {
                     BuildingEdge(0, 1, 100, 80),
                     BuildingEdge(0, 1, 101, 77),
             },
             2),
    Testcase("Graph with 3 nodes",
             {
                     BuildingEdge(0, 1, 10, 120),
                     BuildingEdge(0, 2, 18, 18),
                     BuildingEdge(1, 2, 22, 31),
             },
             3),
    Testcase("Graph with 3 nodes (some bidirectional)",
             {
                     BuildingEdge(0, 1, 100, 120),
                     BuildingEdge(0, 2, 25, 11),
                     BuildingEdge(1, 0, 110, 140),
                     BuildingEdge(1, 2, 44, 133),
                     BuildingEdge(2, 1, 47, 112),
             },
             3),
    Testcase("Graph with 5 nodes",
             {
                     BuildingEdge(0, 1, 300, 100),
                     BuildingEdge(0, 2, 200, 100),
                     BuildingEdge(1, 3, 300, 100),
                     BuildingEdge(1, 4, 500, 100),
                     BuildingEdge(2, 3, 200, 100),
                     BuildingEdge(3, 4, 250, 100),
                     BuildingEdge(1, 0, 300, 100),
                     BuildingEdge(2, 0, 200, 100),
                     BuildingEdge(3, 1, 300, 100),
                     BuildingEdge(4, 1, 500, 100),
                     BuildingEdge(3, 2, 200, 100),
                     BuildingEdge(4, 3, 250, 100),
             },
             5),
    Testcase("Graph with 3 nodes to check charging",
             {
                     BuildingEdge(0, 2, 500, 100),
                     BuildingEdge(0, 1, 300, 100),
                     BuildingEdge(1, 2, 300, 100),
                     BuildingEdge(2, 0, 450, 100),
                     BuildingEdge(1, 0, 250, 100),
                     BuildingEdge(2, 1, 200, 100),
             },
             3)
};

std::vector<Testcase> RKCases = {
    Testcase("Graph with 3 nodes",
             {
                     BuildingEdge(0, 2, 500, 100),
                     BuildingEdge(0, 1, 300, 100),
                     BuildingEdge(1, 2, 300, 100),
                     BuildingEdge(2, 0, 450, 100),
                     BuildingEdge(1, 0, 250, 100),
                     BuildingEdge(2, 1, 200, 100),
             },
             3,
             { 0, 1, 2 }),
};

#endif//EVRP_GRAPH_CASES_HPP

#pragma clang diagnostic pop