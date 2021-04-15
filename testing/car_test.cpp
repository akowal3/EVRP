//
// Created by Andrzej Kowalewski on 15/04/2021.
//

#include <car.h>
#include <catch.hpp>

TEST_CASE("Car temp", "[CAR]") {
    Car c(8);
    REQUIRE(c.t == 8);
}

TEST_CASE("Car fail", "[CAR]") {
    Car c(8);
    REQUIRE(c.t == 8);
}
