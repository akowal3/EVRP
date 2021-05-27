//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include <car.hpp>
#include <utils.hpp>

#include <routingkit/constants.h>

#include <cmath>
#include <vector>

Car::Car(double soc_initial) :
    soc_initial(soc_initial) {
    // Tesla Model 3 Long Range Dual Motor from https://ev-database.org/car/1321/Tesla-Model-3-Long-Range-Dual-Motor
    this->battery_capacity = 70.0;// (kWh) battery usable
    this->soc_min = 0.1;
    this->soc_max = 0.95;
    this->CrossSectionalArea = 2.2;// cross sectional area in m2
    this->RollingResistanceCoeff = 0.007;
    this->DragCoeff = 0.23;
    this->Mass = 2000;// kg
    this->IdleConsumption = 1.5;
    this->DriveTrainEfficiency = 0.95;// in percentage
    this->ChargerProfiles = {
        { charger_type::FAST_175KW, ChargerProfile::FastCharger(90.0) },
        { charger_type::SLOW_50KW, ChargerProfile::FastCharger(45.0) },
    };
    this->soc_min_final = this->soc_min;
}

Car::Car(double soc_initial,
         double soc_min,
         double soc_max,
         double soc_min_final,
         double battery_capacity,
         double CrossSectionalArea,
         double RollingResistanceCoeff,
         double DragCoeff,
         int Mass,
         double IdleConsumption,
         double DriveTrainEfficiency,
         std::unordered_map<charger_type, ChargerProfile> ChargerProfiles) :
    soc_initial(soc_initial),
    soc_min(soc_min),
    soc_max(soc_max),
    soc_min_final(soc_min_final),
    battery_capacity(battery_capacity),
    CrossSectionalArea(CrossSectionalArea),
    RollingResistanceCoeff(RollingResistanceCoeff),
    DragCoeff(DragCoeff),
    Mass(Mass),
    IdleConsumption(IdleConsumption),
    DriveTrainEfficiency(DriveTrainEfficiency),
    ChargerProfiles(std::move(ChargerProfiles)){};


Car Car::TeslaModel3(double soc_initial, double soc_min, double soc_max, double soc_min_final) {
    // Tesla Model 3 Long Range Dual Motor from https://ev-database.org/car/1321/Tesla-Model-3-Long-Range-Dual-Motor
    return Car(soc_initial, soc_min, soc_max, soc_min_final,
               70.0, 2.2,
               0.007, 0.23, 2000, 1.5, 0.95,
               {
                       { charger_type::FAST_175KW, ChargerProfile::FastCharger(90.0) },
                       { charger_type::SLOW_50KW, ChargerProfile::FastCharger(45.0) },
               });
}

Car Car::RenaultZoe(double soc_initial, double soc_min, double soc_max, double soc_min_final) {
    // Tesla Model 3 Long Range Dual Motor from https://ev-database.org/car/1321/Tesla-Model-3-Long-Range-Dual-Motor
    return Car(soc_initial, soc_min, soc_max, soc_min_final,
               70.0, 2.2,
               0.007, 0.23, 2000, 1.5, 0.95,
               {
                       { charger_type::FAST_175KW, ChargerProfile::FastCharger(90.0) },
                       { charger_type::SLOW_50KW, ChargerProfile::FastCharger(45.0) },
               });
}


bool Car::can_traverse(const Edge &e) const {
    return soc_cmp(power_left(e), OP::GREATER_EQUAL, min_charge_level());
}

bool Car::can_traverse(const Edge &e, double initialSoC) const {
    return can_traverse(e, initialSoC, soc_min);
}

bool Car::can_traverse(const Edge &e, double initialSoC, double required_endSoC) const {
    return soc_cmp(power_left(e, initialSoC), OP::GREATER_EQUAL, battery_capacity * required_endSoC);
}

bool Car::can_traverse_final(const Edge &e, double initialSoC) const {
    return can_traverse(e, initialSoC, soc_min_final);
}

bool Car::can_traverse_with_max_soc(const Edge &e) const {
    return can_traverse(e, this->soc_max);
}

bool Car::can_traverse_with_max_soc_final(const Edge &e) const {
    return can_traverse_final(e, this->soc_max);
}

bool Car::will_charge(const Edge &e) const {
    return soc_cmp(power_left(e) / battery_capacity, OP::SMALLER, e.end_charge_level());
}

bool Car::will_charge(const Edge &e, double initialSoC, double endSoC) const {
    return soc_cmp(power_left(e, initialSoC) / battery_capacity, OP::SMALLER, endSoC);
}

// Used for Graph traversal when edge contains all the information about the SoC at a given point
Time Car::get_charge_time(const Edge &e) const {
    return get_charge_time(e.destinationCharger(), power_left(e) / battery_capacity, e.end_charge_level());
}

// Used for Router traversal when SoC information is given as a parameter
Time Car::get_charge_time_to_max(const Edge &e, double initialSoC) const {
    return get_charge_time(e.sourceCharger(), initialSoC, this->soc_max);
}

// The most general implementation, when information about destinationCharger is given in edge and initial and required SoCs are given as input parameters
Time Car::get_charge_time(const Node *chargingStation, double initialSoC, double endSoC) const {
    if (soc_cmp(initialSoC, OP::GREATER_EQUAL, endSoC))
        return 0;// no charging required

    const ChargerProfile &charger = ChargerProfiles.at(chargingStation->best_compatible_type(*this));
    return charger.get_charging_time(initialSoC, endSoC, battery_capacity);
}

// This function calculates the time necessary to charge a car in order to traverse the edge
Time Car::get_charge_time_to_traverse(const Edge &e, double initialSoC) const {
    return get_min_required_charge_time_to_traverse(e, initialSoC, soc_min);
}

Time Car::get_min_required_charge_time_to_traverse(const Edge &e, double initialSoC, double endSoC) const {
    double requiredSoC = consumed_power(e) / battery_capacity + endSoC;

    if (requiredSoC > 1.0) throw "SoC required to traverse this edge is greater than battery capacity";

    return get_charge_time(e.sourceCharger(), initialSoC, requiredSoC);
}

Time Car::get_charge_time_to_traverse_final(const Edge &e, double initialSoC) const {
    return get_min_required_charge_time_to_traverse(e, initialSoC, soc_min_final);
}

double Car::power_left(const Edge &e) const {
    return power_left(e, e.start_charge_level());
}

double Car::power_left(const Edge &e, double initialSoC) const {
    return battery_capacity * initialSoC - consumed_power(e);
}

double Car::consumed_power(const Edge &e) const {
    double consumption_rate = this->calculate_consumption_rate(e.get_speed());//Wh/km
    return consumption_rate * e.get_distance() / 1000;                        //kWh
}

double Car::consumed_soc(const Edge &e) const {
    return consumed_power(e) / battery_capacity;
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

    // If will_charge == false it means that the remaining_soc at the edge will be lower than current soc_left of the car.
    // This is fine because if a car can traverse the edge with 70% charge, it can traverse it with 75% or 82%.
    // Therefore there is no charge time and total_time == e.get_travel_time(). However, it means that when presenting the result
    // we need to account for that and adjust the actual calculated time by traversing the shortest path again and subtracting the time.
    // It's hacky but should work...

    Time total_time = can_traverse(e) ? (will_charge(e) ? e.get_travel_time() + get_charge_time(e) : e.get_travel_time())
                                      : RoutingKit::inf_weight;

    return total_time;
}

double Car::min_charge_level() const {
    return battery_capacity * soc_min;
}

double Car::soc_left(const Edge &e, double initialSoC) const {
    return power_left(e, initialSoC) / battery_capacity;
}

double Car::soc_left_from_max_soc(const Edge &e) const {
    return power_left(e, this->soc_max) / battery_capacity;
}

double Car::max_soc() const {
    return this->soc_max;
}
double Car::min_soc() const {
    return this->soc_min;
}
double Car::min_soc_final() const {
    return this->soc_min_final;
}

double Car::initial_soc() const {
    return this->soc_initial;
}
