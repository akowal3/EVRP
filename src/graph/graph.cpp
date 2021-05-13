//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include <cassert>
#include <graph.hpp>
#include <label.hpp>
#include <queue>
#include <unordered_set>

std::vector<double> Graph::SPEED_STEPS = std::vector<double>{ 0.7, 0.8, 0.9, 1.0 };
std::vector<double> Graph::CHARGER_STEPS = std::vector<double>{ 0.7, 0.8, 0.9, 1. };// 0 is needed as it will be the destination to the final destinationCharger

Graph::Graph(unsigned int charger_count, const std::vector<BuildingEdge> &edges) {

    // Create nodes which correspond to the State of Charge after charging at a given destinationCharger
    this->nodes.reserve(charger_count * CHARGER_STEPS.size());
    for (int id = 0; id < charger_count * CHARGER_STEPS.size(); id++) {
        this->nodes.emplace_back(Node(id, chargeAt(id)));
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
    this->head.reserve(edges.size());// potentially unnecessary
    this->tail.reserve(edges.size());// potentially unnecessary
    for (const Edge &e : this->edges) {
        this->weights.emplace_back([&e](const Car &c) { return c.traverse(e); });
        this->head.emplace_back(e.headID());
        this->tail.emplace_back(e.tailID());
    }

    // Allow to view private parts from outside of the class testing purposes
    this->lookup_edges = &this->edges;
    this->lookup_nodes = &this->nodes;
    this->lookup_weights = &this->weights;
}
Graph::~Graph() = default;

unsigned Graph::eval(int i, const Car &c) const {
    // TODO: parallelize?
    auto time_calculator = this->weights.at(i);
    unsigned total_time = time_calculator(c);
    return total_time;
}

std::vector<unsigned> Graph::evaluate_with(const Car &c) const {
    std::vector<unsigned> evaluated_weights;
    evaluated_weights.reserve(edge_size());
    for (int i = 0; i < edge_size(); i++) {
        evaluated_weights.emplace_back(eval(i, c));
    }
    return evaluated_weights;
}

unsigned Graph::node_size() const {
    return this->nodes.size();
}

unsigned Graph::edge_size() const {
    return this->edges.size();
}

unsigned Graph::originalID(unsigned nodeID) {
    return nodeID / CHARGER_STEPS.size();
}

unsigned Graph::chargerOffset(unsigned nodeID) {
    return nodeID % CHARGER_STEPS.size();
}

double Graph::chargeAt(unsigned nodeID) {
    return Graph::CHARGER_STEPS[chargerOffset(nodeID)];
}

std::vector<unsigned> Graph::ID_to_nodes(unsigned nodeID) {
    std::vector<unsigned> all_nodes;
    all_nodes.reserve(CHARGER_STEPS.size());
    for (int i = 0; i < CHARGER_STEPS.size(); i++) {
        all_nodes.emplace_back(nodeID * CHARGER_STEPS.size() + i);
    }
    return all_nodes;
}
