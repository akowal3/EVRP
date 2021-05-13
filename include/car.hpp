//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_CAR_HPP
#define EVRP_CAR_HPP

class Edge;
class Node;
#include <edge.hpp>
#include <node.hpp>
#include <utils.hpp>


class Car {

public:
    Car(double soc_initial = 1.0);// start with full battery by default
    bool can_traverse(const Edge &e) const;
    bool can_traverse(const Edge &e, double initialSoC) const;
    bool can_traverse_with_max_soc(const Edge &e) const;
    bool will_charge(const Edge &e) const;
    bool will_charge(const Edge &e, double initialSoC, double endSoC) const;

    Time traverse(const Edge &e) const;
    double consumed_power(const Edge &e) const;
    double consumed_soc(const Edge &e) const;
    double power_left(const Edge &e, double initialSoC) const;
    double power_left(const Edge &e) const;
    double calculate_consumption_rate(double v_kmh) const;
    double min_charge_level() const;

    double soc_left(const Edge &e, double initialSoC) const;
    double soc_left_from_max_soc(const Edge &e) const;

    Time get_charge_time(const Node *charger, double initialSoC, double endSoC) const;
    Time get_charge_time_to_max(const Edge &e, double initialSoC) const;
    Time get_charge_time_to_traverse(const Edge &e, double initialSoC) const;
    Time get_charge_time(const Edge &e) const;

    double max_soc() const;
    double min_soc() const;
    double initial_soc() const;

private:
    double battery_capacity;  // in Wh
    double energy_consumption;// in Wh/km
    double charging_power;    // in kW
    double soc_min;           // in percentage (0,1)
    double soc_max;           // in percentage (0,1)
    double soc_initial;       // in percentage (0,1). Initial SoC given by the user
    double range;             // in km
    double charging_rate;     // in %/hour
    double CrossSectionalArea;// in m2
    double RollingResistanceCoeff;
    double DragCoeff;
    int Mass;                   // in kg
    double IdleConsumption;     // in kW
    double DriveTrainEfficiency;// in percentage (0,1)
};


#endif// EVRP_CAR_HPP
