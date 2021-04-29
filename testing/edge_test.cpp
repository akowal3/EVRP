//
// Created by akowal3 on 4/17/21.
//

#include <car.h>
#include <catch.hpp>
#include <edge.h>

TEST_CASE("Edge Basic functionality ", "[EDGE]") {
    Edge e = Edge(123, 655, 10, 120);

    REQUIRE(e.head() == 655);
    REQUIRE(e.tail() == 123);
    REQUIRE(e.get_distance() == 10);
    REQUIRE(e.get_travel_time(1) == 3600.0 * 10.0 / 120.0);
    REQUIRE(e.get_travel_time(0.8) == 3600.0 * 10.0 / (0.8 * 120.0));
}

TEST_CASE("Edge total time test ", "[EDGE]") {
    Car c = Car();
    Edge e = Edge(0, 1, 10, 120);

    REQUIRE(e.get_travel_time(1.) == 300);
    REQUIRE(e.get_travel_time(.8) == 375);
    REQUIRE(e.get_total_time(c, 1.) < e.get_total_time(c, .8));
    REQUIRE(e.get_total_time(c, 1.) == c.get_charge_time(e) + 300);
    REQUIRE(e.get_total_time(c, .8) == c.get_charge_time(e) + 375);
}
