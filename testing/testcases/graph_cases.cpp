#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

//
// Created by akowal3 on 10/05/2021.
//

#include <edge.hpp>
#include <graph_cases.hpp>
#include <testcase.hpp>
#include <vector>

std::vector<Testcase> BuildGraphCases = {
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
                     BuildingEdge(0, 1, 200, 100),
                     BuildingEdge(1, 2, 350, 100),
                     BuildingEdge(2, 0, 450, 100),
                     BuildingEdge(1, 0, 250, 100),
                     BuildingEdge(2, 1, 200, 100),
             },
             3)
};


std::vector<Testcase> RKCases = {
    Testcase("Triangle graph choose to charge",
             {
                     BuildingEdge(0, 2, 500, 100),
                     BuildingEdge(0, 1, 200, 100),
                     BuildingEdge(1, 2, 350, 100),
             },
             3,

             Path({
                          { 0, 0.9 },
                          { 1, 0.9 },
                          { 2, 0.7 },
                  },
                  { 100.0, 100.0 })),
    Testcase("Straight line",

             {
                     BuildingEdge(0, 1, 100, 120),
                     BuildingEdge(1, 2, 25, 11),
                     BuildingEdge(2, 3, 110, 140),
                     BuildingEdge(3, 4, 44, 133),
             },
             5,

             Path({
                          { 0, 0.9 },
                          { 1, 0.8 },
                          { 2, 1.0 },
                          { 3, 0.7 },
                          { 4, 0.7 },
                  },
                  { 120.0, 11.0, 140.0, 133.0 })),
    Testcase("2 straight line",

             {
                     BuildingEdge(0, 1, 100, 120),
                     BuildingEdge(1, 2, 25, 11),
                     BuildingEdge(2, 3, 110, 140),
                     BuildingEdge(3, 4, 44, 133),
                     BuildingEdge(0, 5, 70, 100),
                     BuildingEdge(5, 6, 70, 100),
                     BuildingEdge(6, 7, 70, 100),
                     BuildingEdge(7, 4, 70, 100),

             },
             8,

             Path({
                          { 0, 0.9 },
                          { 5, 0.8 },
                          { 6, 0.8 },
                          { 7, 0.8 },
                          { 4, 0.7 },
                  },
                  { 100.0, 100.0, 100.0, 100.0 })),

    Testcase("2 straight line with shortcuts",

             {
                     BuildingEdge(0, 1, 100, 120),
                     BuildingEdge(1, 2, 25, 11),
                     BuildingEdge(2, 3, 110, 140),
                     BuildingEdge(3, 4, 44, 133),
                     BuildingEdge(0, 5, 70, 100),
                     BuildingEdge(5, 6, 70, 100),
                     BuildingEdge(6, 7, 150, 100),
                     BuildingEdge(7, 4, 100, 100),
                     BuildingEdge(6, 2, 100, 70),
                     BuildingEdge(6, 3, 100, 70),

             },
             8,

             Path({
                          { 0, 0.9 },
                          { 5, 0.8 },
                          { 6, 0.8 },
                          { 3, 0.8 },
                          { 4, 0.7 },
                  },
                  { 100.0, 100.0, 70.0, 133.0 })),

    Testcase("Triangle graph choose to drive slower",

             {
                     BuildingEdge(0, 2, 550, 100),
                     BuildingEdge(0, 1, 400, 100),
                     BuildingEdge(1, 2, 400, 100),
             },
             3,

             Path({
                          { 0, 1 },
                          { 2, 0.7 },
                  },
                  { 70.0 })),

    Testcase("Choose to drive slower",

             {
                     BuildingEdge(0, 1, 350, 120),
                     BuildingEdge(1, 2, 450, 120),
                     BuildingEdge(1, 3, 400, 100),
                     BuildingEdge(3, 2, 400, 100),
             },
             4,

             Path({
                          { 0, 1 },
                          { 1, 1 },
                          { 2, 0.7 },
                  },
                  { 108.0, 84.0 })),

};

#pragma clang diagnostic pop