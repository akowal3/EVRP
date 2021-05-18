//
// Created by akowal3 on 14/05/2021.
//

#ifndef EVRP_CHARGER_PROFILE_HPP
#define EVRP_CHARGER_PROFILE_HPP

#include <utils.hpp>
#include <vector>

class ChargerProfileRange {
    friend class ChargerProfile;

private:
    double start_soc;
    double end_soc;
    double average_charging_power;

public:
    ChargerProfileRange(double start, double end, double charging_power);
    Time get_charging_time(double initialSoC, double endSoC, double battery_capacity) const;
};

// models charging data from ev database as a set of ranges and associated average charging power
// for example fast charger (175kW) has 90kW average power on 10%-80% range and 11kW otherwise
// calculating the charging time is sum of intersections between charging need and the profile
class ChargerProfile {
private:
    std::vector<ChargerProfileRange> profile;

public:
    ChargerProfile(const std::vector<std::pair<double, double>> &socs, const std::vector<double> &powers);
    ChargerProfile(std::vector<ChargerProfileRange> p);
    static ChargerProfile FastCharger(double avg_fast_charging_power, double avg_slow_charging_power = 11.0);
    static ChargerProfile SlowCharger(double voltage, double current, double phases);
    Time get_charging_time(double initialSoC, double endSoC, double battery_capacity) const;
};

#endif//EVRP_CHARGER_PROFILE_HPP
