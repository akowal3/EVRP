//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_GRAPH_HPP
#define EVRP_GRAPH_HPP


#include <functional>
#include <vector>

#include "car.hpp"
#include "edge.hpp"

class Graph {
public:
    Graph(unsigned node_count, const std::vector<BuildingEdge> &edges);
    ~Graph();
    std::vector<unsigned> head;
    std::vector<unsigned> tail;
    unsigned eval(int i, const Car &c) const;
    std::vector<unsigned int> evaluate_with(const Car &c) const;


    static std::vector<double> SPEED_STEPS;
    static std::vector<double> CHARGER_STEPS;

    const std::vector<Node> *lookup_nodes;
    const std::vector<Edge> *lookup_edges;
    const std::vector<std::function<Time(const Car &)>> *lookup_weights;

    unsigned int edge_size() const;
    unsigned int node_size() const;

    static unsigned int originalID(unsigned int nodeID);
    static unsigned int chargerOffset(unsigned int nodeID);
    static double chargeAt(unsigned int nodeID);

    static std::vector<unsigned int> ID_to_nodes(unsigned int nodeID);

private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<std::function<Time(const Car &)>> weights;
};


#endif//EVRP_GRAPH_HPP
