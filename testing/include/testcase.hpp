//
// Created by akowal3 on 03/05/2021.
//

#ifndef EVRP_TESTCASE_HPP
#define EVRP_TESTCASE_HPP

#include <edge.hpp>
#include <graph.hpp>

#include <cassert>
#include <string>
#include <utility>
#include <vector>

class Path {
public:
    std::vector<unsigned> nodes;
    std::vector<double> charge_levels;
    std::vector<double> speeds;
    Path(const std::vector<std::pair<unsigned, double>> &path, std::vector<double> speeds) :
        speeds(std::move(speeds)) {
        nodes.reserve(path.size());
        charge_levels.reserve(path.size());
        for (auto &node : path) {
            nodes.emplace_back(node.first);
            charge_levels.emplace_back(node.second);
        }
    }
    Path() = default;

    unsigned source_original() const {
        return nodes.front();
    }

    unsigned source() const {

        unsigned start_node = nodes.front();

        auto charge_step = std::find(Graph::CHARGER_STEPS.begin(), Graph::CHARGER_STEPS.end(), charge_levels.front());

        assert(charge_step != Graph::CHARGER_STEPS.end());

        unsigned charge_offset = charge_step - Graph::CHARGER_STEPS.begin();

        return start_node + charge_offset;
    }

    unsigned target() const {
        return nodes.back();
    }

    std::vector<unsigned> get_node_path() const {
        std::vector<unsigned> node_path;
        node_path.reserve(nodes.size());

        for (int i = 0; i < nodes.size(); i++) {
            auto charge_step = std::find(Graph::CHARGER_STEPS.begin(), Graph::CHARGER_STEPS.end(), charge_levels[i]);

            assert(charge_step != Graph::CHARGER_STEPS.end());

            unsigned charge_offset = charge_step - Graph::CHARGER_STEPS.begin();

            node_path.emplace_back(nodes[i] * Graph::CHARGER_STEPS.size() + charge_offset);
        }


        return node_path;
    }
};


class Testcase {
public:
    std::string description;
    std::vector<BuildingEdge> graph;
    int node_count;
    Path path;

    Testcase(std::string description, std::vector<BuildingEdge> graph, int node_count, Path path = Path()) :
        graph(std::move(graph)),
        description(std::move(description)),
        node_count(node_count),
        path(std::move(path)){};
};


#endif//EVRP_TESTCASE_HPP
