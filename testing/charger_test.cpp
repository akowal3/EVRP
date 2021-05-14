//
// Created by akowal3 on 14/05/2021.
//

#include <catch.hpp>
#include <charger_profile.hpp>
#include <iostream>


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
        auto left = ChargerProfilePoint(tc.x0, tc.y0);
        auto right = ChargerProfilePoint(tc.x1, tc.y1);
        auto interpolated = ChargerProfilePoint::interpolate(left, tc.soc, right);
        double expected = tc.y0 + ((tc.y1 - tc.y0) / (tc.x1 - tc.x0)) * (tc.soc - tc.x0);
        REQUIRE(expected == Approx(interpolated.get_charging_power()).epsilon(1e-9));
    }
}