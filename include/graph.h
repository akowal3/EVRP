//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_GRAPH_H
#define EVRP_GRAPH_H


#include <functional>
#include <vector>

#include "car.h"
#include "edge.h"

class Graph {
public:
    Graph(unsigned node_count, const std::vector<BuildingEdge> &edges);
    ~Graph();
    std::vector<unsigned> head;
    std::vector<unsigned> tail;
    unsigned eval(int i, const Car &c) const;
    unsigned size() const;

    static std::vector<double> SPEED_STEPS;
    static std::vector<double> CHARGER_STEPS;

    const std::vector<Node> *lookup_nodes;
    const std::vector<Edge> *lookup_edges;
    const std::vector<std::function<Time(const Car &)>> *lookup_weights;

private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<std::function<Time(const Car &)>> weights;
};


#endif//EVRP_GRAPH_H
