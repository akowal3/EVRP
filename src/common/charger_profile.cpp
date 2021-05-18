//
// Created by akowal3 on 14/05/2021.
//

#include <cassert>
#include <charger_profile.hpp>
#include <utility>

ChargerProfile::ChargerProfile(std::vector<ChargerProfileRange> p) :
    profile(std::move(p)) {}

ChargerProfile::ChargerProfile(const std::vector<std::pair<double, double>> &socs, const std::vector<double> &powers) {
    assert(socs.size() == powers.size());
    profile.reserve(socs.size());
    for (int i = 0; i < socs.size(); i++) {
        assert(socs[i].first < socs[i].second);
        profile.emplace_back(ChargerProfileRange(socs[i].first, socs[i].second, powers[i]));
    }
}

Time ChargerProfile::get_charging_time(double initialSoC, double endSoC, double battery_capacity) const {
    assert(initialSoC < endSoC);
    Time charging_time = 0;
    for (auto &range : profile) {
        charging_time += range.get_charging_time(initialSoC, endSoC, battery_capacity);
    }
    return charging_time;
}

ChargerProfile ChargerProfile::FastCharger(double avg_fast_charging_power, double avg_slow_charging_power) {
    return ChargerProfile(
            { { 0.0, 0.1 }, { 0.1, 0.8 }, { 0.8, 1.0 } },
            { avg_slow_charging_power, avg_fast_charging_power, avg_slow_charging_power });
}

ChargerProfile ChargerProfile::SlowCharger(double voltage, double current, double phases) {
    return ChargerProfile(
            { { 0.0, 1.0 } },
            { voltage * current * phases / 1000.0 });
}

Time ChargerProfileRange::get_charging_time(double initialSoC, double endSoC, double battery_capacity) const {
    if (initialSoC > end_soc || start_soc > endSoC) {
        return 0;// no charging in this range
    }

    double intersection_start = std::max(initialSoC, start_soc);
    double intersection_end = std::min(endSoC, end_soc);
    double chargingTime = (intersection_end - intersection_start) * battery_capacity / average_charging_power;

    return Time(3600.0 * chargingTime);
}

ChargerProfileRange::ChargerProfileRange(double start, double end, double charging_power) :
    start_soc(start), end_soc(end), average_charging_power(charging_power) {}
