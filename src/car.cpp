//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include "car.h"

#include <routingkit/constants.h>

#include <cmath>
#include <vector>

Car::Car() {
    // Tesla Model 3 Long Range Dual Motor from https://ev-database.org/car/1321/Tesla-Model-3-Long-Range-Dual-Motor
    this->battery_capacity = 70.0;            // (kWh) battery usable
    this->energy_consumption = 131.0 / 1000.0;// (Wh/km) Combined - Mild Weather
    this->charging_power = 85.0;              // (kW)  Supercharver v2 150kW
    this->range = 535.0;                      // (km) Combined - Mild Weather
    this->soc_min = 0.1;
    this->charging_rate = charging_power / battery_capacity;
    this->CrossSectionalArea = 2.2;// cross sectional area in m2
    this->RollingResistanceCoeff = 0.007;
    this->DragCoeff = 0.23;
    this->Mass = 2000;// kg
    this->IdleConsumption = 1.5;
    this->DriveTrainEfficiency = 0.95;// in percentage
}

bool Car::can_traverse(const Edge &e) const {
    return power_left(e) >= battery_capacity * soc_min;
}

bool Car::will_charge(const Edge &e) const {
    return power_left(e) / battery_capacity < e.end_charge_level();
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
    double consumption_rate = this->calculate_consumption_rate(e.get_speed());
    return consumption_rate * e.get_distance() / 1000;//kWh
}

double Car::calculate_consumption_rate(double v_kmh) const {
    double v_ms = v_kmh / 3.6;
    double slope = 0;// to be changed by edge
    double G = 9.81;

    double F_rolling = this->Mass * G * this->RollingResistanceCoeff;
    double F_drag = std::pow(v_ms, 2) * this->DragCoeff * this->CrossSectionalArea;
    double F_slope = this->Mass * G * std::sin(slope);

    double P = (F_rolling + F_drag + F_slope) * this->DriveTrainEfficiency * v_ms + (this->IdleConsumption * 1000);// W

    return P / v_kmh;// Wh/km
}

Time Car::traverse(const Edge &e) const {

    // If will_charge == false it means that the SoC at the edge will be lower than current SoC of the car.
    // This is fine because if a car can traverse the edge with 70% charge, it can traverse it with 75% or 82%.
    // Therefore there is no charge time and total_time == e.get_travel_time(). However, it means that when presenting the result
    // we need to account for that and adjust the actual calculated time by traversing the shortest path again and subtracting the time.
    // It's hacky but should work...

    Time total_time = can_traverse(e) ? (will_charge(e) ? e.get_travel_time() + get_charge_time(e) : e.get_travel_time())
                                      : RoutingKit::inf_weight;

    return total_time;
}
