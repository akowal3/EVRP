//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include "car.h"

#include <routingkit/constants.h>

#include <vector>

Car::Car(double batteryCapacity, double energyConsumption, double chargingPower, double socMin, double socMax, double range) {
    this->battery_capacity = batteryCapacity;
    this->energy_consumption = energyConsumption;
    this->charging_power = chargingPower;
    this->soc_min = socMin;
    this->range = range;
}

Car::Car() {
    // Tesla Model 3 Long Range Dual Motor from https://ev-database.org/car/1321/Tesla-Model-3-Long-Range-Dual-Motor
    this->battery_capacity = 70.0;            // (kWh) battery usable
    this->energy_consumption = 131.0 / 1000.0;// (Wh/km) Combined - Mild Weather
    this->charging_power = 85.0;              // (kW)  Supercharver v2 150kW
    this->range = 535.0;                      // (km) Combined - Mild Weather
    this->soc_min = 0.1;
}

bool Car::can_traverse(const Edge &e) const {
    return power_left(e) >= battery_capacity * soc_min;
}

Time Car::get_charge_time(const Edge &e) const {
    // Time to recharge the battery to max charge_level levels
    //TODO: edge will have information about chargers available and speed they can charge at, so take this into account
    //TODO: model charging as piecewise linear functions instead of linear

    double current_power = power_left(e);
    double end_power = battery_capacity * e.end_charge_level();
    double charging_time = 0.0;// in hours

    std::vector<double> rate_modifier = { 1.0, 0.7, 0.5 };
    std::vector<double> soc_max = { 0.7, 0.9, 1.0 };

    for (int i = 0; i < rate_modifier.size(); i++) {
        double additional_charge = (soc_max[i] * battery_capacity <= end_power)
                                           ? soc_max[i] * battery_capacity - current_power
                                           : end_power - current_power;
        charging_time += (additional_charge / (charging_power * rate_modifier[i]));
        current_power += additional_charge;

        if (current_power == end_power)
            break;
    }

    return Time(3600.0 * charging_time);// in seconds
}

inline double Car::power_left(const Edge &e) const {
    return battery_capacity * e.start_charge_level() - consumed_power(e);
}

inline double Car::consumed_power(const Edge &e) const {//kWh
    // TODO: take into account energy consumption at different speeds
    return this->energy_consumption * e.get_distance();
}

Time Car::traverse(const Edge &e) const {
    if (can_traverse(e)) {
        return e.get_travel_time() + get_charge_time(e);
    }

    return Time(RoutingKit::inf_weight);
}
