//
// Created by Andrzej Kowalewski on 15/04/2021.
//

#include <car.h>
#include <edge.h>

#include <catch.hpp>

TEST_CASE("Car can traverse", "[CAR]") {
    Car c = Car();

    Node n1 = Node(0, 0.8);
    Node n2 = Node(1, 0.7);
    Edge e1 = Edge(&n1, &n2, 10, 120);
    Edge e2 = Edge(&n1, &n2, 10000, 120);

    WHEN("Distance is small") {
        THEN("Default car can traverse the edge") {
            REQUIRE(c.can_traverse(e1) == true);
        }
    }
    
    WHEN("Distance is large") {
        THEN("Default car can't traverse the edge") {
            REQUIRE(c.can_traverse(e2) == false);
        }
    }
}
