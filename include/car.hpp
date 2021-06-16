//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_CAR_H
#define EVRP_CAR_H

class Edge;
class Node;
#include <charger_profile.hpp>
#include <edge.hpp>
#include <node.hpp>
#include <utils.hpp>

#include <unordered_map>

typedef std::unordered_map<charger_type, ChargerProfile> ChargerProfileMap;

class Car {

public:
    Car(double soc_initial = 1.0);// start with full battery by default
    Car(double soc_initial, double soc_min, double soc_max, double soc_min_final, double battery_capacity, double CrossSectionalArea, double RollingResistanceCoeff, double DragCoeff, int Mass, double IdleConsumption, double DriveTrainEfficiency, std::unordered_map<charger_type, ChargerProfile> ChargerProfiles, Time charging_overhead);

    static Car TeslaModel3(double soc_initial, double soc_min, double soc_max, double soc_min_final, unsigned charging_overhead);
    static Car RenaultZoe(double soc_initial, double soc_min, double soc_max, double soc_final_min, unsigned charging_overhead);

    bool can_traverse(const Edge &e) const;
    bool can_traverse(const Edge &e, double initialSoC) const;
    bool can_traverse(const Edge &e, double initialSoC, double required_endSoC) const;
    bool can_traverse_with_max_soc(const Edge &e) const;
    bool can_traverse_final(const Edge &e, double initialSoC) const;
    bool can_traverse_with_max_soc_final(const Edge &e) const;
    bool will_charge(const Edge &e) const;
    bool will_charge(const Edge &e, double initialSoC, double endSoC) const;

    bool can_charge(const Node *n) const;
    bool can_charge(const Edge &e) const;

    Time traverse(const Edge &e) const;
    double consumed_power(const Edge &e) const;
    double consumed_soc(const Edge &e) const;
    double power_left(const Edge &e, double initialSoC) const;
    double power_left(const Edge &e) const;
    double calculate_consumption_rate(double v_kmh) const;
    double min_charge_level() const;

    double soc_left(const Edge &e, double initialSoC) const;
    double soc_left_from_max_soc(const Edge &e) const;

    Time get_charge_time(const Node *chargingStation, double initialSoC, double endSoC) const;
    Time get_charge_time_to_max(const Edge &e, double initialSoC) const;
    Time get_charge_time_to_traverse(const Edge &e, double initialSoC) const;
    Time get_charge_time(const Edge &e) const;
    Time get_min_required_charge_time_to_traverse(const Edge &e, double initialSoC, double endSoC) const;
    Time get_charge_time_to_traverse_final(const Edge &e, double initialSoC) const;

    double max_soc() const;
    double min_soc() const;
    double min_soc_final() const;
    double initial_soc() const;

private:
    double battery_capacity;  // in Wh
    double soc_min;           // in percentage (0,1)
    double soc_max;           // in percentage (0,1)
    double soc_initial;       // in percentage (0,1). Initial SoC given by the user
    double soc_min_final;     // in percentage (0,1). Final SoC given by the user
    double CrossSectionalArea;// in m2
    double RollingResistanceCoeff;
    double DragCoeff;
    int Mass;                   // in kg
    double IdleConsumption;     // in kW
    double DriveTrainEfficiency;// in percentage (0,1)
    ChargerProfileMap ChargerProfiles;
    Time charging_overhead_time;// in s
};


#endif// EVRP_CAR_H
