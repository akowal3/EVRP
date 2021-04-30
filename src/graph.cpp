//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include "graph.h"
#include <cassert>

std::vector<double> Graph::SPEED_STEPS = std::vector<double>{0.8, 0.9, 1.0, 1.1};
std::vector<double> Graph::CHARGER_STEPS = std::vector<double>{0.8, 1};

Graph::Graph(unsigned int charger_count, const std::vector<BuildingEdge> &edges) {

    // Create nodes which correspond to the State of Charge after charging at a given charger
    this->nodes.reserve(charger_count * CHARGER_STEPS.size());
    for (int id = 0; id < charger_count * CHARGER_STEPS.size(); id++) {
        this->nodes.emplace_back(Node(id, CHARGER_STEPS[id % CHARGER_STEPS.size()]));
    }

    // Crete edges corresponding to different speeds of traveling between the chargers
    this->edges.reserve(CHARGER_STEPS.size() * CHARGER_STEPS.size() * SPEED_STEPS.size() * edges.size());
    for (const BuildingEdge &e : edges) {
        unsigned charge_levels = CHARGER_STEPS.size();
        for (int i = 0; i < charge_levels; i++) {
            unsigned source = e.from * charge_levels + i;
            for (int j = 0; j < charge_levels; j++) {
                unsigned destination = e.to * charge_levels + j;
                for (auto speed_modifier : SPEED_STEPS) {
                    this->edges.emplace_back(Edge(&nodes[source], &nodes[destination],
                                                  e.distance, e.max_speed * speed_modifier));
                }
            }
        }
    }

    // Create weights, which will be evaluated on runtime when a query with new car comes in
    this->weights.reserve(edges.size());
    for (const Edge &e : this->edges) {
        this->weights.emplace_back([&e](const Car &c) { return c.traverse(e); });
    }

    // Allow to view private parts from outside of the class testing purposes
    this->lookup_edges = &this->edges;
    this->lookup_nodes = &this->nodes;
    this->lookup_weights = &this->weights;
}
Graph::~Graph() = default;

unsigned Graph::eval(int i, const Car &c) const {
    auto time_calculator = this->weights.at(i);
    unsigned total_time = time_calculator(c);
    return total_time;
}

unsigned Graph::size() const {
    assert(this->tail.size() == this->head.size());
    assert(this->tail.size() == this->weights.size());
    return this->tail.size();
}
