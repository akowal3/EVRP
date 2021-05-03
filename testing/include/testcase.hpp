//
// Created by akowal3 on 03/05/2021.
//

#ifndef EVRP_TESTCASE_HPP
#define EVRP_TESTCASE_HPP

#include <edge.h>

#include <utility>
#include <vector>


class Testcase {
public:
    std::string description;
    std::vector<BuildingEdge> graph;
    int node_count;
    std::vector<unsigned> expected_path;

    Testcase(std::string description, std::vector<BuildingEdge> graph, int node_count, std::vector<unsigned> expected_path = {})
        : graph(std::move(graph)),
          description(std::move(description)),
          node_count(node_count),
          expected_path(std::move(expected_path)){};
};


#endif//EVRP_TESTCASE_HPP
