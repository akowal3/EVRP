//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_CAR_H
#define EVRP_CAR_H

class Edge;
#include <edge.h>

class Car {

public:
    Car();
    bool can_traverse(const Edge &e) const;
    Time get_charge_time(const Edge &e) const;
    Time traverse(const Edge &e) const;
    double consumed_power(const Edge &e) const;
    double power_left(const Edge &e) const;
    bool will_charge(const Edge &e) const;
    double calculate_consumption_rate(double v_kmh) const;
    double min_charge_level() const;

private:
    double battery_capacity;  // in Wh
    double energy_consumption;// in Wh/km
    double charging_power;    // in kW
    double soc_min;           // in percentage
    double range;             // in km
    double charging_rate;     // in %/hour
    double CrossSectionalArea;// in m2
    double RollingResistanceCoeff;
    double DragCoeff;
    int Mass;                   // in kg
    double IdleConsumption;     // in kW
    double DriveTrainEfficiency;// in percentage (0,1)
};


#endif// EVRP_CAR_H
