//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_EDGE_HPP
#define EVRP_EDGE_HPP

class Car;
#include <car.hpp>
#include <node.hpp>
#include <utils.hpp>

class Edge {
private:
    unsigned distance;
    double speed;
    const Node *start;
    const Node *end;

public:
    Edge(const Node *from, const Node *to, unsigned int distance, double speed);
    Time get_travel_time() const;
    unsigned get_distance() const;
    double get_speed() const;
    unsigned tailID() const;
    unsigned headID() const;
    double start_charge_level() const;
    double end_charge_level() const;
    const Node *destinationCharger() const;
    const Node *sourceCharger() const;
};

// Dummy datastructure used to create simple graph by hand
class BuildingEdge {
public:
    unsigned from;
    unsigned to;
    unsigned distance;
    unsigned max_speed;
    BuildingEdge(unsigned from, unsigned to, unsigned distance, unsigned max_speed);
    Time get_travel_time(double speed_modifier) const;
};

#endif//EVRP_EDGE_HPP
