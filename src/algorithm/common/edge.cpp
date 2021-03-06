//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include <edge.hpp>

#include "routingkit/constants.h"

Edge::Edge(const Node *from, const Node *to, unsigned distance, double speed) :
    start(from),
    end(to),
    distance(distance),
    speed(speed) {}

Time Edge::get_travel_time() const {
    double res = 3600.0 * ((double) distance / (double) speed);
    return Time(res);
}

unsigned Edge::tailID() const {
    return this->start->id();
}

unsigned Edge::headID() const {
    return this->end->id();
}
unsigned Edge::get_distance() const {
    return this->distance;
}
double Edge::start_charge_level() const {
    return this->start->soc();
}
double Edge::end_charge_level() const {
    return this->end->soc();
}
double Edge::get_speed() const {
    return this->speed;
}
const Node *Edge::destinationCharger() const {
    return this->end;
}

const Node *Edge::sourceCharger() const {
    return this->start;
}

bool operator==(const Edge &left, const Edge &right) {
    if (left.start && right.start && left.end && right.end) {
        return *left.start == *right.start && *left.end == *right.end &&
               left.distance == right.distance && left.speed == right.speed;
    }
    return false;
}

BuildingEdge::BuildingEdge(unsigned from, unsigned to, unsigned distance, unsigned max_speed) :
    from(from),
    to(to),
    distance(distance),
    max_speed(max_speed) {}

Time BuildingEdge::get_travel_time(double speed_modifier) const {
    double res = 3600.0 * ((double) distance / (speed_modifier * (double) max_speed));
    return Time(res);
}