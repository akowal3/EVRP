//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include "edge.h"
#include "routingkit/constants.h"

Edge::Edge(unsigned _from, unsigned _to, unsigned _distance) : from(_from), to(_to), distance(_distance) {}

Time Edge::get_travel_time() const {
    return distance;
}
Time Edge::get_charge_time() const {
    return 0;
}
Time Edge::get_total_time() const {
    return get_travel_time() + get_charge_time();
}

Time Edge::get_total_time(const Car &c) const {
    if (get_energy(c) < 7)//c.capacity)
        return Time(RoutingKit::inf_weight);

    return get_travel_time() + get_charge_time() + c.t;
}

Energy Edge::get_energy(const Car &c) const {
    return 8;
}
unsigned Edge::tail() const {
    return this->from;
}

unsigned Edge::head() const {
    return this->to;
}
