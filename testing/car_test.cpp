//
// Created by Andrzej Kowalewski on 15/04/2021.
//

#include <car.h>
#include <catch.hpp>
#include <edge.h>

TEST_CASE("Car can traverse", "[CAR]") {
    Car c = Car();
    Edge e1 = Edge(0, 1, 10, 120);
    Edge e2 = Edge(0, 1, 10000, 120);
    REQUIRE(c.can_traverse(e1) == true);
    REQUIRE(c.can_traverse(e2) == false);
}

TEST_CASE("Car can traverse2", "[CAR]") {
    Car c = Car();
    Edge e1 = Edge(0, 1, 10, 120);
    Edge e2 = Edge(0, 1, 10000, 120);
    REQUIRE(c.can_traverse(e1) == true);
    REQUIRE(c.can_traverse(e2) == false);
}