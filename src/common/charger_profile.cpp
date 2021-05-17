//
// Created by akowal3 on 14/05/2021.
//

#include <cassert>
#include <charger_profile.hpp>

ChargerProfilePoint::ChargerProfilePoint(double soc, double power) :
    soc(soc), charging_power(power) {}

ChargerProfilePoint ChargerProfilePoint::interpolate(const ChargerProfilePoint &left, double soc, const ChargerProfilePoint &right) {
    double x0 = left.soc;
    double y0 = left.charging_power;
    double x1 = right.soc;
    double y1 = right.charging_power;
    return ChargerProfilePoint(soc, (y0 * (x1 - soc) + y1 * (soc - x0)) / (x1 - x0));
}

double ChargerProfilePoint::average_charging_rate(const ChargerProfilePoint &left, const ChargerProfilePoint &right) {
    return 0.5 * (right.charging_power + left.charging_power) * (right.soc - left.soc);
}

bool ChargerProfilePoint::operator<(const ChargerProfilePoint &other) const {
    return this->soc < other.soc;
}
bool ChargerProfilePoint::operator<=(const ChargerProfilePoint &other) const {
    return this->soc <= other.soc;
}
bool ChargerProfilePoint::operator<(double soc) const {
    return this->soc < soc;
}
bool ChargerProfilePoint::operator>=(double soc) const {
    return this->soc >= soc;
}
bool ChargerProfilePoint::operator==(const ChargerProfilePoint &other) const {
    return (this->soc == other.soc) && (this->charging_power == other.charging_power);
}
double ChargerProfilePoint::get_charging_power() const {
    return charging_power;
}

double ChargerProfilePoint::get_soc() const {
    return soc;
}

ChargerProfile::ChargerProfile(const std::vector<ChargerProfilePoint> &p) :
    profile(std::move(p)) {}

ChargerProfile::ChargerProfile(const std::vector<double> &socs, const std::vector<double> &powers) {
    assert(socs.size() == powers.size());

    profile.reserve(socs.size());
    for (int i = 0; i < socs.size(); i++) {
        profile.emplace_back(ChargerProfilePoint(socs[i], powers[i]));
    }
}

double ChargerProfile::charging_power(double initialSoC, double endSoC) const {
    assert(initialSoC < endSoC);
    switch (profile.size()) {
        case 0:
            return 0.0;
        case 1:
            return profile[0].charging_power;
        default:
            double average_rate = 0;
            for (int i = 0; i < profile.size(); i++) {
                const ChargerProfilePoint &current = profile[i];
                const ChargerProfilePoint &next = profile[i + 1];

                if (next < initialSoC)// ignore points before initialSoC
                    continue;

                if (current < initialSoC && next >= initialSoC) {
                    auto initialPoint = ChargerProfilePoint::interpolate(current, initialSoC, next);
                    average_rate += ChargerProfilePoint::average_charging_rate(initialPoint, next);
                    continue;
                }

                if (current < endSoC && next >= endSoC) {
                    auto finalPoint = ChargerProfilePoint::interpolate(current, endSoC, next);
                    average_rate += ChargerProfilePoint::average_charging_rate(current, finalPoint);
                    break;
                }

                average_rate += ChargerProfilePoint::average_charging_rate(current, next);
            }
            return average_rate / (endSoC - initialSoC);
    }
}

Time ChargerProfile::charging_time(double initialSoC, double endSoC, double battery_capacity) const {
    assert(initialSoC < endSoC);
    switch (profile.size()) {
        case 0:
            return 0.0;
        case 1:
            return Time(profile[0].charging_power * 3600.0);
        default:
            double chargingTime = 0.0;// in hours

            for (int i = 0; i < profile.size(); i++) {
                const ChargerProfilePoint &current = profile[i];
                const ChargerProfilePoint &next = profile[i + 1];

                if (next < initialSoC)// ignore points before initialSoC
                    continue;

                if (current < initialSoC && next >= initialSoC) {
                    auto initialPoint = ChargerProfilePoint::interpolate(current, initialSoC, next);
                    auto average_rate = ChargerProfilePoint::average_charging_rate(initialPoint, next);
                    chargingTime += (next.soc - initialPoint.soc) * battery_capacity / average_rate;
                    continue;
                }

                if (current < endSoC && next >= endSoC) {
                    auto finalPoint = ChargerProfilePoint::interpolate(current, endSoC, next);
                    auto average_rate = ChargerProfilePoint::average_charging_rate(current, finalPoint);
                    chargingTime += (finalPoint.soc - current.soc) * battery_capacity / average_rate;
                    break;
                }

                auto average_rate = ChargerProfilePoint::average_charging_rate(current, next);
                chargingTime += (next.soc - current.soc) * battery_capacity / average_rate;
            }
            return Time(3600.0 * chargingTime);
    }
}

std::ostream &operator<<(std::ostream &os, const ChargerProfilePoint &p) {
    return os << "(" << p.soc << ", " << p.charging_power << ")";
}
