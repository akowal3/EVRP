//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_EDGE_H
#define EVRP_EDGE_H


#include "car.h"
#include <unordered_map>

typedef unsigned Time;
typedef double Energy;

class Edge {
private:
    //    std::unordered_map<unsigned, Time> roads;// speed limit: length
    unsigned from;
    unsigned to;
    unsigned distance;
    double end_charge;
    unsigned max_speed;
    Time get_travel_time() const;
    Time get_charge_time() const;
    Energy get_energy(const Car &c) const;

public:
    Edge(unsigned _from, unsigned _to, unsigned _distance);
    Time get_total_time() const;
    Time get_total_time(const Car &c) const;
    unsigned tail() const;
    unsigned head() const;
};


#endif//EVRP_EDGE_H
