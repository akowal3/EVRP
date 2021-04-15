//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_EDGE_H
#define EVRP_EDGE_H

typedef unsigned Time;// (s)

class Car;
#include <car.h>

class Edge {
private:
    unsigned from;
    unsigned to;
    unsigned distance;
    unsigned max_speed;

public:
    Edge(unsigned _from, unsigned _to, unsigned _distance, unsigned _max_speed);
    Time get_total_time(const Car &c, double speed_modifier) const;
    Time get_travel_time(double speed_modifier) const;
    unsigned get_distance() const;
    unsigned tail() const;
    unsigned head() const;
};


#endif//EVRP_EDGE_H
