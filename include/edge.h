//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_EDGE_H
#define EVRP_EDGE_H

typedef unsigned Time;// (s)

class Car;
#include <car.h>
#include <node.h>

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
    unsigned tail() const;
    unsigned head() const;
    double start_soc() const;
    double end_soc() const;
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

#endif//EVRP_EDGE_H
