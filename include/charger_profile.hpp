//
// Created by akowal3 on 14/05/2021.
//

#ifndef EVRP_CHARGER_PROFILE_HPP
#define EVRP_CHARGER_PROFILE_HPP

#include <utils.hpp>
#include <vector>


class ChargerProfilePoint {
    friend class ChargerProfile;
    friend std::ostream &operator<<(std::ostream &os, const ChargerProfilePoint &p);

private:
    double soc;           // in percentage (0,1)
    double charging_power;// in kW
public:
    ChargerProfilePoint(double soc, double power);
    static ChargerProfilePoint interpolate(const ChargerProfilePoint &left, double soc, const ChargerProfilePoint &right);
    static double average_charging_rate(const ChargerProfilePoint &left, const ChargerProfilePoint &right);
    bool operator<(const ChargerProfilePoint &other) const;
    bool operator<=(const ChargerProfilePoint &other) const;
    bool operator==(const ChargerProfilePoint &other) const;
    bool operator<(double soc) const;
    bool operator>=(double soc) const;
    double get_charging_power() const;
};

class ChargerProfile {
private:
    std::vector<ChargerProfilePoint> profile;

public:
    ChargerProfile(const std::vector<ChargerProfilePoint> &p);
    double charging_power(double initialSoC, double endSoC) const;
    ChargerProfile(const std::vector<double> &socs, const std::vector<double> &powers);
};


std::ostream &operator<<(std::ostream &os, const ChargerProfilePoint &p);

#endif//EVRP_CHARGER_PROFILE_HPP
