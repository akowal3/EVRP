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
    return 0.5 * (left.charging_power + right.charging_power) * (right.soc - left.soc);
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

std::ostream &operator<<(std::ostream &os, const ChargerProfilePoint &p) {
    return os << "(" << p.soc << ", " << p.charging_power << ")";
}
