//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include "edge.h"
#include "routingkit/constants.h"

Edge::Edge(unsigned _from, unsigned _to, unsigned _distance, unsigned _max_speed) : from(_from), to(_to), distance(_distance), max_speed(_max_speed) {}

Time Edge::get_travel_time(double speed_modifier) const {
    return Time(3600.0 * distance / (max_speed * speed_modifier));
}

Time Edge::get_total_time(const Car &c, double speed_modifier) const {

    if (!c.can_traverse(*this)) return Time(RoutingKit::inf_weight);

    return get_travel_time(speed_modifier) + c.get_charge_time(*this);
}

unsigned Edge::tail() const {
    return this->from;
}

unsigned Edge::head() const {
    return this->to;
}
unsigned Edge::get_distance() const {
    return this->distance;
}
