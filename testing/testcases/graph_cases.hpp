#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by akowal3 on 01/05/2021.
//

#ifndef EVRP_GRAPH_CASES_HPP
#define EVRP_GRAPH_CASES_HPP

#include <edge.h>

#include <vector>

constexpr unsigned SpeedConfigCount = 3;
std::vector<double> TEST_speed_step[SpeedConfigCount] = {
    { 0.7, 0.8, 0.9 },
    { 1.0 },
    { 0.4, 0.44, 0.812, 0.11, 1.12 },
};

constexpr unsigned ChargerConfigCount = 3;
std::vector<double> TEST_charger_step[ChargerConfigCount] = {
    { 0.8, 1.0 },
    { 0.7, 0.8, 0.9 },
    { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 },
};

constexpr unsigned TestCount = 6;
unsigned TEST_node_count[TestCount] = { 2, 2, 2, 3, 3, 5 };
std::vector<BuildingEdge> TEST_edges[TestCount] = {
    {
            BuildingEdge(0, 1, 100, 80),
    },
    {
            BuildingEdge(0, 1, 100, 80),
            BuildingEdge(1, 0, 100, 80),
    },
    {
            BuildingEdge(0, 1, 100, 80),
            BuildingEdge(0, 1, 101, 77),
    },
    {
            BuildingEdge(0, 1, 10, 120),
            BuildingEdge(0, 2, 18, 18),
            BuildingEdge(1, 2, 22, 31),
    },
    {
            BuildingEdge(0, 1, 100, 120),
            BuildingEdge(0, 2, 25, 11),
            BuildingEdge(1, 0, 110, 140),
            BuildingEdge(1, 2, 44, 133),
            BuildingEdge(2, 1, 47, 112),
    },
    {
            BuildingEdge(0, 1, 100, 80),
            BuildingEdge(0, 2, 70, 120),
            BuildingEdge(1, 3, 50, 100),
            BuildingEdge(1, 4, 120, 100),
            BuildingEdge(2, 3, 110, 60),
            BuildingEdge(3, 4, 20, 120),
            BuildingEdge(1, 0, 100, 80),
            BuildingEdge(2, 0, 70, 120),
            BuildingEdge(3, 1, 50, 100),
            BuildingEdge(4, 1, 120, 100),
            BuildingEdge(3, 2, 110, 60),
            BuildingEdge(4, 3, 20, 120),
    }
};
std::string TEST_description[TestCount] = {
    "Graph with single edge",
    "Graph with single bidirectional edge",
    "Graph with 2 same edges",
    "Graph with 3 nodes",
    "Graph with 3 nodes (some bidirectional)",
    "Graph with 5 nodes",
};

#endif//EVRP_GRAPH_CASES_HPP

#pragma clang diagnostic pop