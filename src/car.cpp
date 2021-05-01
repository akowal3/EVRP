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
    this->charging_rate = charging_power / battery_capacity;
}

Car::Car() {
    // Tesla Model 3 Long Range Dual Motor from https://ev-database.org/car/1321/Tesla-Model-3-Long-Range-Dual-Motor
    this->battery_capacity = 70.0;            // (kWh) battery usable
    this->energy_consumption = 131.0 / 1000.0;// (Wh/km) Combined - Mild Weather
    this->charging_power = 85.0;              // (kW)  Supercharver v2 150kW
    this->range = 535.0;                      // (km) Combined - Mild Weather
    this->soc_min = 0.1;
    this->charging_rate = charging_power / battery_capacity;
}

bool Car::can_traverse(const Edge &e) const {
    return power_left(e) >= battery_capacity * soc_min;
}

Time Car::get_charge_time(const Edge &e) const {
    //TODO: edge will have information about chargers available and speed they can charge at, so take this into account

    double current_level = power_left(e) / battery_capacity;
    double end_level = e.end_charge_level();
    double charging_time = 0.0;// in hours

    std::vector<double> rate_modifier = { 1.0, 0.7, 0.5 };
    std::vector<double> cutoff_level = { 0.7, 0.9, 1.0 };

    for (int i = 0; i < rate_modifier.size(); i++) {
        double additional_charge = (cutoff_level[i] <= end_level) ? cutoff_level[i] - current_level
                                                                  : end_level - current_level;
        charging_time += additional_charge / (charging_rate * rate_modifier[i]);
        current_level += additional_charge;

        if (current_level == end_level)
            break;// reached desired charge. No need to charge further.
    }

    return Time(3600.0 * charging_time);// in seconds
}

inline double Car::power_left(const Edge &e) const {
    return battery_capacity * e.start_charge_level() - consumed_power(e);
}

inline double Car::consumed_power(const Edge &e) const {//kWh
    // TODO: take into account energy consumption at different speeds
    double consumption_modifier = (e.get_speed() > 90.0) ? 1.15 : 1.0;// assume that at speed >90km/h consumption is 15% higher. Change it!!!
    return this->energy_consumption * e.get_distance() * consumption_modifier;
}

Time Car::traverse(const Edge &e) const {
    if (can_traverse(e)) {
        return e.get_travel_time() + get_charge_time(e);
    }

    return Time(RoutingKit::inf_weight);
}
