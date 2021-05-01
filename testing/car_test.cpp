//
// Created by Andrzej Kowalewski on 15/04/2021.
//

#include <car.h>
#include <edge.h>
#include <routingkit/constants.h>

#include <catch.hpp>

TEST_CASE("Charging time", "[CAR]") {
    Car c = Car();
    Node n1 = Node(0, 0.8);

    WHEN("Charging to 70%") {
        Node n2 = Node(1, 0.7);
        Edge e = Edge(&n1, &n2, 120, 120);

        REQUIRE(c.can_traverse(e) == true);
        REQUIRE(c.consumed_power(e) == 120 * 131.0 / 1000.0);
        REQUIRE(c.get_charge_time(e) == Time(3600 * 8.72 / 85.0));
    }

    WHEN("Charging to 74% (not cutoff)") {
        Node n2 = Node(1, 0.74);
        Edge e = Edge(&n1, &n2, 120, 120);

        REQUIRE(c.can_traverse(e) == true);
        REQUIRE(c.consumed_power(e) == 120 * 131.0 / 1000.0);
        REQUIRE(c.get_charge_time(e) == Time(3600 * (8.72 / 85.0 + 2.8 / (85.0 * 0.7))));
    }
}

TEST_CASE("Edge traversal", "[CAR]") {
    Car c = Car();

    Node n1 = Node(0, 0.8);
    Node n2 = Node(1, 0.7);
    Edge e1 = Edge(&n1, &n2, 120, 120);
    Edge e2 = Edge(&n1, &n2, 10000, 120);

    WHEN("Distance is small") {
        THEN("Default car can traverse the edge") {
            REQUIRE(c.can_traverse(e1) == true);
            REQUIRE(c.traverse(e1) == Time(3600 * 8.72 / 85.0 + 3600));
            REQUIRE(c.traverse(e1) == c.get_charge_time(e1) + e1.get_travel_time());
        }
    }

    WHEN("Distance is large") {
        THEN("Default car can't traverse the edge") {
            REQUIRE(c.can_traverse(e2) == false);
            REQUIRE(c.traverse(e2) == RoutingKit::inf_weight);
        }
    }
}
