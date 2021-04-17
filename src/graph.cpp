//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include "graph.h"
#include <cassert>

std::vector<double> Graph::SPEED_STEPS = std::vector<double>{0.8, 0.9, 1.0, 1.1};

Graph::Graph(unsigned int node_count, const std::vector<Edge> &edges) {
    // assuming we charge to 100% can we reach the edge
    this->weights = new std::vector<std::function<Time(const Car &)>>;
    for (const Edge &edge : edges) {
        for (auto modifier : SPEED_STEPS) {
            this->tail.push_back(edge.tail());
            this->head.push_back(edge.head());
            this->weights->push_back([modifier, edge](const Car &c) {
                return edge.get_total_time(c, modifier);
            });
        }
    }
}
Graph::~Graph() {
    delete this->weights;
}

unsigned Graph::eval(int i, const Car &c) const {
    return this->weights->at(i)(c);
}
unsigned Graph::size() const {
    assert(this->tail.size() == this->head.size());
    assert(this->tail.size() == this->weights->size());
    return this->tail.size();
}
