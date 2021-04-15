//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_GRAPH_H
#define EVRP_GRAPH_H


#include "car.h"
#include "edge.h"
#include <functional>
#include <vector>

class Graph {
public:
    Graph(unsigned node_count, const std::vector<Edge> &edges);
    ~Graph();
    std::vector<unsigned> head;
    std::vector<unsigned> tail;
    unsigned eval(int i, const Car &c) const;
    unsigned size() const;

private:
    std::vector<Edge> edges;
    std::vector<std::function<Time(const Car &)>> *weights;

    static constexpr int CHARGER_STEPS = 4;
    static std::vector<double> SPEED_STEPS;
};


#endif//EVRP_GRAPH_H
