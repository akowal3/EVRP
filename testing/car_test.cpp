//
// Created by Andrzej Kowalewski on 15/04/2021.
//

#include <car.h>
#include <edge.h>
#include <routingkit/constants.h>

#include <catch.hpp>
#include <custom_macros.hpp>
#include <iostream>
#include <sstream>


TEST_CASE("Edge traversal", "[CAR]") {
    WHEN("Tesla Model 3 is a car") {
        Car c = Car();
        THEN("Can drive 50 km at any reasonable speed") {
            Node start = Node(0, 0.8);
            Node end = Node(1, 0.7);
            unsigned distance = 50;

            WHEN("Will not need charge") {

                for (auto speed = 20; speed < 93; speed++) {
                    Edge e = Edge(&start, &end, distance, speed);
                    REQUIRE(c.can_traverse(e) == true);
                    REQUIRE(c.traverse(e) != RoutingKit::inf_weight);
                    REQUIRE(c.will_charge(e) == false);
                    REQUIRE(c.traverse(e) == e.get_travel_time());
                }
            }

            WHEN("Will need to charge at the edge") {
                for (auto speed = 93; speed < 200; speed++) {
                    Edge e = Edge(&start, &end, distance, speed);
                    REQUIRE(c.can_traverse(e) == true);
                    REQUIRE(c.traverse(e) != RoutingKit::inf_weight);
                    REQUIRE(c.will_charge(e) == true);
                    REQUIRE(c.traverse(e) == e.get_travel_time() + c.get_charge_time(e));
                }
            }
        }

        THEN("Will always have to charge if SoC at the end == 1") {
            Node start = Node(0, 0.8);
            Node end = Node(1, 1);
            unsigned distance = 100;
            for (auto speed = 50; speed < 200; speed++) {
                Edge e = Edge(&start, &end, distance, speed);
                REQUIRE(c.can_traverse(e) == true);
                REQUIRE(c.traverse(e) != RoutingKit::inf_weight);
                REQUIRE(c.will_charge(e) == true);
                REQUIRE(c.traverse(e) == e.get_travel_time() + c.get_charge_time(e));
            }
        }
    }
}
