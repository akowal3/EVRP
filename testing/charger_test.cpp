//
// Created by akowal3 on 14/05/2021.
//

#include <catch.hpp>
#include <charger_profile.hpp>
#include <iostream>
#include <node.hpp>


TEST_CASE("ChargerProfile operators", "[CHARGER]") {

    std::vector<double> l = { 0.0, 0.4, 0.22, 0.455, 0.622, 0.6135, 0.6542 };
    std::vector<double> r = { 0.124, 0.122, 0.4, 0.122, 0.33, 0.45, 0.1112 };

    REQUIRE(l.size() == r.size());

    for (int i = 0; i < l.size(); i++) {
        auto lval = l[i];
        auto rval = r[i];
        REQUIRE((ChargerProfilePoint(lval, 111) <= ChargerProfilePoint(rval, 124)) == (lval <= rval));
        REQUIRE((ChargerProfilePoint(rval, 11.23) <= ChargerProfilePoint(lval, 12.24)) == (rval <= lval));
        REQUIRE((ChargerProfilePoint(rval, 634.11) <= ChargerProfilePoint(rval, 41.2)) == (rval <= rval));

        REQUIRE((ChargerProfilePoint(lval, 111) < ChargerProfilePoint(rval, 124)) == (lval < rval));
        REQUIRE((ChargerProfilePoint(rval, 11.23) < ChargerProfilePoint(lval, 12.24)) == (rval < lval));
        REQUIRE((ChargerProfilePoint(rval, 634.11) < ChargerProfilePoint(rval, 41.2)) == (rval < rval));

        REQUIRE((ChargerProfilePoint(lval, 532) >= rval) == (lval >= rval));
        REQUIRE((ChargerProfilePoint(rval, 12.221) >= rval) == (rval >= rval));
        REQUIRE((ChargerProfilePoint(lval, 532) >= lval) == (lval >= lval));

        REQUIRE(ChargerProfilePoint(lval, 531) == ChargerProfilePoint(lval, 531));
        REQUIRE(ChargerProfilePoint(rval, 420.69) == ChargerProfilePoint(rval, 420.69));
        REQUIRE((ChargerProfilePoint(lval, 5112) == ChargerProfilePoint(lval, 2115)) == false);
        REQUIRE((ChargerProfilePoint(lval, 5112) == ChargerProfilePoint(rval, 5112)) == false);
    }
}


TEST_CASE("ChargerPoint interpolation", "[CHARGER]") {

    typedef struct {
        double x0;
        double y0;
        double x1;
        double y1;
        double soc;
    } InterpolationTestCase;

    std::vector<InterpolationTestCase> cases = {
        { 0, 0, 1, 1, 0.5 },
        { 0.4, 12.54, 0.5, 44.1, 0.44 },
        { 0.22, 512.122, 0.5, 112.1, 0.22 },
        { 0.11, 55.12, 0.98, 15.1, 0.98 },
        { 0.22, 51.1282, 0.512, 112.1, 0.399 },
    };

    for (auto &tc : cases) {
        // verify whether testcase is valid
        REQUIRE(tc.x0 >= 0.0);
        REQUIRE(tc.x0 <= 1.0);
        REQUIRE(tc.x1 >= 0.0);
        REQUIRE(tc.x1 <= 1.0);
        REQUIRE(tc.soc >= 0.0);
        REQUIRE(tc.soc <= 1.0);

        auto left = ChargerProfilePoint(tc.x0, tc.y0);
        auto right = ChargerProfilePoint(tc.x1, tc.y1);
        auto interpolated = ChargerProfilePoint::interpolate(left, tc.soc, right);
        double expected = tc.y0 + ((tc.y1 - tc.y0) / (tc.x1 - tc.x0)) * (tc.soc - tc.x0);
        REQUIRE(expected == Approx(interpolated.get_charging_power()).epsilon(1e-9));
        REQUIRE(interpolated.get_soc() == tc.soc);
    }
}

TEST_CASE("ChargerPoint average power", "[CHARGER]") {

    typedef struct {
        std::vector<double> socs;
        std::vector<double> powers;
        double initialSoC;
        double endSoC;
        double expected;
    } AvgPowerTestCase;

    std::vector<AvgPowerTestCase> cases = {
        { std::vector<double>{ 0.0, 1.0 },
          std::vector<double>{ 0.0, 1.0 },
          0.0, 1.0, 0.5 },
        { std::vector<double>{ 0.0, 1.0 },
          std::vector<double>{ 2, 4 },
          0.0, 1.0, 3 },
        { std::vector<double>{ 0.0, 1.0 },
          std::vector<double>{ 22.245, 22.245 },
          0.0, 1.0, 22.245 },
        { std::vector<double>{ 0.0, 0.5, 1.0 },
          std::vector<double>{ 0.0, 1.0, 3.0 },
          0.0, 1.0, 1.25 },
        { std::vector<double>{ 0.0, 0.5, 1.0 },
          std::vector<double>{ 0.0, 5.0, 15.0 },
          0.0, 1.0, 6.25 },
        { std::vector<double>{ 0.0, 0.5, 1.0 },
          std::vector<double>{ 0.0, 5.0, 15.0 },
          0.0, 0.7, 26.5 / 7.0 },
    };

    for (auto &tc : cases) {
        auto profile = ChargerProfile(tc.socs, tc.powers);
        auto power = profile.charging_power(tc.initialSoC, tc.endSoC);

        REQUIRE(power == Approx(tc.expected).epsilon(1e-9));
    }
}

TEST_CASE("Charge time is additive", "[CAR, CHARGER]") {
    Car c = Car();
    auto KW175 = ChargerProfile(
            { 0.0, 0.1, 0.2, 0.52, 0.61, 0.9, 1.0 },
            { 0.0, 135.0, 140.0, 148.0, 110.0, 30.0, 0.0 });
    auto KW50 = ChargerProfile(
            { 0.0, 0.07, 0.5, 0.83, 0.9, 1.0 },
            { 0.0, 40.0, 50.0, 50.0, 25.0, 0.0 });

    auto charger = Node(0, 1.0, charger_type::FAST_175KW);
    auto time_40_to_70 = c.get_charge_time(&charger, 0.3, 0.4);
    auto time_70_to_90 = c.get_charge_time(&charger, 0.4, 0.7);
    auto time_90_to_100 = c.get_charge_time(&charger, 0.9, 1.0);

    std::cout << time_40_to_70 << ", " << time_70_to_90 << ", " << c.get_charge_time(&charger, 0.3, 0.7) << std::endl;

    CHECK((time_40_to_70 + time_70_to_90) == c.get_charge_time(&charger, 0.3, 0.7));
    CHECK(time_cmp(time_40_to_70 + time_70_to_90, OP::EQUAL, c.get_charge_time(&charger, 0.4, 0.9)));
    CHECK(time_cmp(time_40_to_70 + time_70_to_90 + time_90_to_100, OP::EQUAL, c.get_charge_time(&charger, 0.4, 1.0)));

    double initialSoC = 0.4;
    double endSoC = 0.7;
    double average_charging_power = KW175.charging_power(initialSoC, endSoC);// kW
    double battery_capacity = 70.0;

    double charging_time = (endSoC - initialSoC) * battery_capacity / average_charging_power;
    Time charging_time_s = Time(charging_time * 3600.0);

    Time charging_time2 = KW175.charging_time(initialSoC, endSoC, battery_capacity);

    CHECK(KW175.charging_time(initialSoC, endSoC, battery_capacity) == KW175.charging_time(initialSoC, 0.6, battery_capacity) + KW175.charging_time(0.6, endSoC, battery_capacity));

    std::cout << "Based on average: " << charging_time_s << ", based on calculation: " << charging_time2 << std::endl;
}