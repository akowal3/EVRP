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
    std::vector<unsigned> head;
    std::vector<unsigned> tail;
    ~Graph();
    unsigned eval(int i, const Car &c);
    unsigned size();

private:
    std::vector<Edge> edges;
    std::vector<std::function<Time(const Car &)>> *weights;

    static constexpr int CHARGER_STEPS = 4;
    static constexpr int SPEED_STEPS = 4;
};


#endif//EVRP_GRAPH_H
