//
// Created by akowal3 on 14/05/2021.
//

#include <catch.hpp>
#include <charger_profile.hpp>
#include <iostream>
#include <node.hpp>

TEST_CASE("ChargerProfileRange", "[CHARGER]") {
    typedef struct {
        double start_soc;
        double end_soc;
        double avg_power;
        double initialSoC;
        double endSoC;
        double battery_capacity;
        double expected_hours;
    } ChargingTimeCase;

    std::vector<ChargingTimeCase> cases = {
        { 0.0, 0.5, 11.0,
          0.7, 0.9, 70.0,
          0.0 },
        { 0.3, 0.5, 11.0,
          0.2, 0.25, 70.0,
          0.0 },
        { 0.3, 0.5, 11.0,
          0.1, 0.7, 70.0,
          0.2 * 70.0 / 11.0 },
    };

    for (auto &tc : cases) {
        auto range = ChargerProfileRange(tc.start_soc, tc.end_soc, tc.avg_power);
        auto time_secs = range.get_charging_time(tc.initialSoC, tc.endSoC, tc.battery_capacity);
        REQUIRE(time_secs / 3600.0 == Approx(tc.expected_hours).epsilon(1e-2));
    }
}


TEST_CASE("ChargerProfile charging time", "[CHARGER]") {

    typedef struct {
        std::vector<std::pair<double, double>> socs;
        std::vector<double> powers;
        double initialSoC;
        double endSoC;
        double battery_capacity;
        double expected_hours;
    } ChargingTimeCase;

    std::vector<ChargingTimeCase> cases = {
        { std::vector<std::pair<double, double>>{ { 0.0, 1.0 } },
          std::vector<double>{ 5.0 },
          0.0, 1.0, 70.0,
          70.0 / 5.0 },
        { std::vector<std::pair<double, double>>{ { 0.0, 0.5 }, { 0.5, 1.0 } },
          std::vector<double>{ 45.0, 120.0 },
          0.2, 0.5, 70.0,
          0.3 * 70 / 45.0 },
        { std::vector<std::pair<double, double>>{ { 0.0, 0.5 }, { 0.5, 1.0 } },
          std::vector<double>{ 45.0, 120.0 },
          0.2, 0.7, 70.0,
          0.3 * 70.0 / 45.0 + 0.2 * 70.0 / 120.0 },
        { std::vector<std::pair<double, double>>{ { 0.0, 0.3 }, { 0.3, 0.5 }, { 0.5, 1.0 } },
          std::vector<double>{ 45.0, 70.0, 120.0 },
          0.2, 0.7, 70.0,
          0.1 * 70.0 / 45.0 + 0.2 * 70.0 / 70.0 + 0.2 * 70.0 / 120.0 },
        { std::vector<std::pair<double, double>>{ { 0.0, 0.1 }, { 0.1, 0.8 }, { 0.8, 1.0 } },
          std::vector<double>{ 11.0, 90.0, 11.0 },
          0.2, 0.7, 120.0,
          0.5 * 120.0 / 90.0 },
        { std::vector<std::pair<double, double>>{ { 0.0, 0.1 }, { 0.1, 0.8 }, { 0.8, 1.0 } },
          std::vector<double>{ 11.0, 90.0, 11.0 },
          0.2, 0.9, 120.0,
          0.6 * 120.0 / 90.0 + 0.1 * 120.0 / 11.0 },
    };

    for (auto &tc : cases) {
        auto profile = ChargerProfile(tc.socs, tc.powers);
        auto time_secs = profile.get_charging_time(tc.initialSoC, tc.endSoC, tc.battery_capacity);
        REQUIRE(time_secs / 3600.0 == Approx(tc.expected_hours).epsilon(1e-2));
    }
}

//TEST_CASE("Charge time is additive", "[CAR, CHARGER]") {
//    Car c = Car();
//    auto KW175 = ChargerProfile::FastCharger(90.0);
//    auto KW50 = ChargerProfile::FastCharger(45.0);
//
//    auto charger = Node(0, 1.0, charger_type::FAST_175KW);
//    auto time_40_to_70 = c.get_charge_time(&charger, 0.3, 0.4);
//    auto time_70_to_90 = c.get_charge_time(&charger, 0.4, 0.7);
//    auto time_90_to_100 = c.get_charge_time(&charger, 0.9, 1.0);
//
//    std::cout << time_40_to_70 << ", " << time_70_to_90 << ", " << c.get_charge_time(&charger, 0.3, 0.7) << std::endl;
//
//    CHECK((time_40_to_70 + time_70_to_90) == c.get_charge_time(&charger, 0.3, 0.7));
//    CHECK(time_cmp(time_40_to_70 + time_70_to_90, OP::EQUAL, c.get_charge_time(&charger, 0.4, 0.9)));
//    CHECK(time_cmp(time_40_to_70 + time_70_to_90 + time_90_to_100, OP::EQUAL, c.get_charge_time(&charger, 0.4, 1.0)));
//
//    double initialSoC = 0.4;
//    double endSoC = 0.7;
//    double average_charging_power = KW175.charging_power(initialSoC, endSoC);// kW
//    double battery_capacity = 70.0;
//
//    double charging_time = (endSoC - initialSoC) * battery_capacity / average_charging_power;
//    Time charging_time_s = Time(charging_time * 3600.0);
//
//    Time charging_time2 = KW175.get_charging_time(initialSoC, endSoC, battery_capacity);
//
//    CHECK(KW175.get_charging_time(initialSoC, endSoC, battery_capacity) == KW175.get_charging_time(initialSoC, 0.6, battery_capacity) + KW175.get_charging_time(0.6, endSoC, battery_capacity));
//
//    std::cout << "Based on average: " << charging_time_s << ", based on calculation: " << charging_time2 << std::endl;
//}