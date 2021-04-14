//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include "graph.h"
#include <cassert>

Graph::Graph(unsigned int node_count, const std::vector<Edge> &edges) {
    // assuming we charge to 100% can we reach the edge
    this->weights = new std::vector<std::function<Time(const Car &)>>;
    for (const Edge &edge : edges) {
        for (int i = 0; i < SPEED_STEPS; ++i) {
            this->tail.push_back(edge.tail());
            this->head.push_back(edge.head());
            this->weights->push_back([i, edge](const Car &c) {
                return (i + 1) * 2 * edge.get_total_time(c);
            });
        }
    }
}
Graph::~Graph() {
    delete this->weights;
}
unsigned Graph::eval(int i, const Car &c) {
    return this->weights->at(i)(c);
}
unsigned Graph::size() {
    assert(this->tail.size() == this->head.size());
    assert(this->tail.size() == this->weights->size());
    return this->tail.size();
}
