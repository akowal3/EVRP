//
// Created by Andrzej Kowalewski on 14/04/2021.
//

#ifndef EVRP_CAR_H
#define EVRP_CAR_H

class Edge;
#include <edge.h>

class Car {

public:
    Car(double batteryCapacity, double energyConsumption, double chargingPower, double socMin, double socMax, double range);
    Car();
    bool can_traverse(const Edge &e) const;
    Time get_charge_time(const Edge &e) const;
    Time traverse(const Edge &e) const;
    double consumed_power(const Edge &e) const;
    double power_left(const Edge &e) const;

private:
    double battery_capacity;  // in Wh
    double energy_consumption;// in Wh/km
    double charging_power;    // in kW
    double soc_min;           // in percentage
    double range;             // in km
    double charging_rate;     // in %/hour
};


#endif// EVRP_CAR_H
