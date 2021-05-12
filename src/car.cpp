//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#include <car.h>
#include <routingkit/constants.h>

#include <cmath>
#include <vector>

// assumes t inputs correspond to SoC values and the SoC value is only correct to 1 decimal place ie. XX.X
bool soc_cmp(double left, OP operand, double right) {
    constexpr static double epsilon = 0.001;
    bool equal = std::abs(left - right) < epsilon;
    bool greater = left - right >= epsilon;
    switch (operand) {
        case OP::GREATER:
            return greater;
        case OP::GREATER_EQUAL:
            return greater || equal;
        case OP::EQUAL:
            return equal;
        case OP::SMALLER:
            return !greater && !equal;
        case OP::SMALLER_EQUAL:
            return !greater;
        case OP::NOT_EQUAL:
            return !equal;
    }
}

bool time_cmp(Time left, OP operand, Time right) {
    constexpr static long epsilon = 60;// treat 1 minute difference as no difference
    bool equal = std::labs((long) left - (long) right) < epsilon;
    bool greater = (long) left - (long) right >= epsilon;
    switch (operand) {
        case OP::GREATER:
            return greater;
        case OP::GREATER_EQUAL:
            return greater || equal;
        case OP::EQUAL:
            return equal;
        case OP::SMALLER:
            return !greater && !equal;
        case OP::SMALLER_EQUAL:
            return !greater;
        case OP::NOT_EQUAL:
            return !equal;
    }
}

Car::Car(double soc_initial) :
    soc_initial(soc_initial) {
    // Tesla Model 3 Long Range Dual Motor from https://ev-database.org/car/1321/Tesla-Model-3-Long-Range-Dual-Motor
    this->battery_capacity = 70.0;            // (kWh) battery usable
    this->energy_consumption = 131.0 / 1000.0;// (Wh/km) Combined - Mild Weather
    this->charging_power = 85.0;              // (kW)  Supercharver v2 150kW
    this->range = 535.0;                      // (km) Combined - Mild Weather
    this->soc_min = 0.1;
    this->soc_max = 0.95;
    this->charging_rate = charging_power / battery_capacity;
    this->CrossSectionalArea = 2.2;// cross sectional area in m2
    this->RollingResistanceCoeff = 0.007;
    this->DragCoeff = 0.23;
    this->Mass = 2000;// kg
    this->IdleConsumption = 1.5;
    this->DriveTrainEfficiency = 0.95;// in percentage
}

bool Car::can_traverse(const Edge &e) const {
    return soc_cmp(power_left(e), OP::GREATER_EQUAL, min_charge_level());
}

bool Car::can_traverse(const Edge &e, double initialSoC) const {
    return soc_cmp(power_left(e, initialSoC), OP::GREATER_EQUAL, min_charge_level());
}

bool Car::can_traverse_with_max_soc(const Edge &e) const {
    return can_traverse(e, this->soc_max);
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
Time Car::get_charge_time(const Node *charger, double initialSoC, double endSoC) const {
    double charging_time = 0.0;// in hours
    double currentSoC = initialSoC;

    //TODO: for now the edge doesn't hold information about the different chargers.
    // Those fixed modifiers and cutoff levels should be part of the Node located at the head of the edge
    std::vector<double> rate_modifier = { 1.0, 0.7, 0.5 };
    std::vector<double> cutoff_level = { 0.7, 0.9, 1.0 };

    for (int i = 0; i < rate_modifier.size(); i++) {
        if (soc_cmp(currentSoC, OP::GREATER_EQUAL, endSoC))
            break;// reached desired charge. No need to charge further.

        if (soc_cmp(cutoff_level[i], OP::SMALLER_EQUAL, currentSoC))
            continue;// skip to the correct charging rate

        double additional_charge = soc_cmp(cutoff_level[i], OP::SMALLER, endSoC)
                                           ? cutoff_level[i] - currentSoC
                                           : endSoC - currentSoC;

        charging_time += additional_charge / (charging_rate * rate_modifier[i]);
        currentSoC += additional_charge;
    }


    return Time(3600.0 * charging_time);// in seconds
}

// This function calculates the time necessary to charge a car in order to traverse the edge
Time Car::get_charge_time_to_traverse(const Edge &e, double initialSoC) const {
    double requiredSoC = consumed_power(e) / battery_capacity + soc_min;

    if (requiredSoC > 1.0) throw "SoC required to traverse this edge is greater than battery capacity";

    return get_charge_time(e.sourceCharger(), initialSoC, requiredSoC);
}

double Car::power_left(const Edge &e) const {
    return power_left(e, e.start_charge_level());
}

double Car::power_left(const Edge &e, double initialSoC) const {
    return battery_capacity * initialSoC - consumed_power(e);
}

double Car::consumed_power(const Edge &e) const {                             //kWh
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

double Car::initial_soc() const {
    return this->soc_initial;
}
