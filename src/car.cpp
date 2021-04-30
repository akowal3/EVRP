//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include "car.h"
#include <routingkit/constants.h>

Car::Car(double batteryCapacity, double energyConsumption, double chargingPower, double socMin, double socMax, double range) {
    this->battery_capacity = batteryCapacity;
    this->energy_consumption = energyConsumption;
    this->charging_power = chargingPower;
    this->soc_min = socMin;
    this->soc_max = socMax;
    this->range = range;
}

Car::Car() {
    // Tesla Model 3 Long Range Dual Motor from https://ev-database.org/car/1321/Tesla-Model-3-Long-Range-Dual-Motor
    this->battery_capacity = 70.0;            // (kWh) battery usable
    this->energy_consumption = 131.0 / 1000.0;// (Wh/km) Combined - Mild Weather
    this->charging_power = 85.0;              // (kW)  Supercharver v2 150kW
    this->range = 535.0;                      // (km) Combined - Mild Weather
    this->soc_min = 0.1;
    this->soc_max = 0.8;
}

bool Car::can_traverse(const Edge &e) const {
    // assuming we start at soc_max
    return power_left(e) >= soc_min;
}

Time Car::get_charge_time(const Edge &e, double charge_modifier) const {
    // Time to recharge the battery to max SoC levels
    //TODO: edge will have information about chargers available and speed they can charge at, so take this into account
    //TODO: model charging as piecewise linear functions instead of linear

    return Time(3600.0 * consumed_power(e) / charging_power);// in seconds
}

inline double Car::power_left(const Edge &e) const {
    return battery_capacity * soc_max - consumed_power(e);
}

inline double Car::consumed_power(const Edge &e) const {
    // TODO: take into account energy consumption at different speeds
    return this->energy_consumption * e.get_distance();
}

Time Car::traverse(const Edge &e) const {
    if (can_traverse(e)) {
        e.get_travel_time() + get_charge_time(e);
    }

    return Time(RoutingKit::inf_weight);
}
