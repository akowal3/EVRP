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


TEST_CASE("Edge traversal for Graph", "[CAR]") {
    GIVEN("Tesla Model 3 as a car") {
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

        THEN("Will always have to charge if soc_left at the end == 1") {
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

        THEN("Can drive 300 km only at slower speeds") {
            Node start = Node(0, 1);
            Node end = Node(1, 1);
            unsigned distance = 300;
            for (auto speed = 30; speed < 126; speed++) {
                Edge e = Edge(&start, &end, distance, speed);
                REQUIRE_FAILMSG(c.can_traverse(e) == true, "@ " << speed << " km/h");
                REQUIRE(c.traverse(e) != RoutingKit::inf_weight);
                REQUIRE(c.will_charge(e) == true);
                REQUIRE(c.power_left(e) >= c.min_charge_level());
                REQUIRE(c.traverse(e) == e.get_travel_time() + c.get_charge_time(e));
            }

            for (auto speed = 126; speed < 200; speed++) {
                Edge e = Edge(&start, &end, distance, speed);
                REQUIRE(c.can_traverse(e) == false);
                REQUIRE(c.traverse(e) == RoutingKit::inf_weight);
                REQUIRE(c.will_charge(e) == true);// power_left is negative?
                REQUIRE(c.power_left(e) <= c.min_charge_level());
                //                REQUIRE(c.traverse(e) == e.get_travel_time() + c.get_charge_time(e));
            }
        }
    }
}

TEST_CASE("SoC greater or equal", "[CAR]") {
    std::vector<double> left = { 0.700, 0.69912314, 0.699, 0.420, 0.420, 0.123 };
    std::vector<double> right = { 0.699, 0.70123123, 0.6999, 0.1, 0.4193256534, 0.1234321127 };
    std::vector<bool> result = { true, false, true, true, true, true };

    REQUIRE(left.size() == right.size());
    REQUIRE(left.size() == result.size());

    for (int i = 0; i < left.size(); i++) {
        auto l = left[i];
        auto r = right[i];
        bool res = result[i];
        THEN(l << " >= " << r << " is " << (res ? "true" : "false")) {
            REQUIRE(soc_cmp(l, OP::GREATER_EQUAL, r) == res);
        }
    }
}

TEST_CASE("Consumed SoC", "[CAR, EDGE]") {
    GIVEN("Tesla Model 3 as a test car") {
        Car c = Car();
        Node start = Node(124);
        Node end = Node(790);

        Edge e = Edge(&start, &end, 10, 100);
        auto rate = c.calculate_consumption_rate(100);
        auto expected_consumed = 10 * rate / 1000.0;//kWh
        REQUIRE(c.consumed_power(e) == expected_consumed);
        REQUIRE(c.consumed_soc(e) == expected_consumed / 70.0);
    }
}

TEST_CASE("Can traverse for Router", "[CAR, ROUTER]") {
    GIVEN("Tesla Model 3 as a test car") {
        Car c = Car();
        Node start = Node(0);
        Node end = Node(1);
        GIVEN("Edge consuming ~85% of battery") {
            Edge e = Edge(&start, &end, 300, 120);
            THEN("Can traverse with max (" << c.max_soc() * 100.0 << "%)") {
                REQUIRE(c.can_traverse_with_max_soc(e));
                REQUIRE(soc_cmp(c.max_soc(), OP::GREATER_EQUAL, c.consumed_soc(e)));
                REQUIRE(soc_cmp(c.max_soc() - c.consumed_soc(e), OP::GREATER_EQUAL, c.min_soc()));
            }
            THEN("Can't traverse with SoC == consumed_energy") {
                auto minimumSoC = c.consumed_soc(e);
                REQUIRE(c.can_traverse(e, minimumSoC) == false);
                REQUIRE(soc_cmp(minimumSoC - c.consumed_soc(e), OP::SMALLER, c.min_soc()));
            }
            THEN("Can traverse with SoC = consumed_energy + min_soc") {
                auto minimumSoC = c.consumed_soc(e) + c.min_soc();
                REQUIRE(c.can_traverse(e, minimumSoC) == true);
                REQUIRE(soc_cmp(minimumSoC - c.consumed_soc(e), OP::GREATER_EQUAL, c.min_soc()));
            }
            std::vector<double> socs = { 0.4, 0.44, 0.51, 0.567, 0.64, 0.76, 0.80, 0.81 };
            for (auto &soc : socs) {
                WHEN("SoC = " << soc * 100.0 << "%") {
                    THEN("Can't traverse the edge") {
                        REQUIRE(c.can_traverse(e, soc) == false);
                        REQUIRE(soc_cmp(c.consumed_soc(e), OP::GREATER_EQUAL, soc));
                        REQUIRE(soc_cmp(soc - c.consumed_soc(e), OP::SMALLER, c.min_soc()));
                    }
                }
            }
        }

        GIVEN("Too long edge") {
            Edge e = Edge(&start, &end, 1000, 200);
            THEN("Can't traverse with max (" << c.max_soc() * 100.0 << "%)") {
                REQUIRE(!c.can_traverse_with_max_soc(e));
                REQUIRE(soc_cmp(c.consumed_soc(e), OP::GREATER_EQUAL, c.max_soc()));
            }
        }

        GIVEN("Edge consuming ~15% of battery") {
            Edge e = Edge(&start, &end, 96, 71);
            THEN("Can traverse with max (" << c.max_soc() * 100.0 << "%)") {
                REQUIRE(c.can_traverse_with_max_soc(e));
                REQUIRE(soc_cmp(c.max_soc(), OP::GREATER, c.consumed_soc(e)));
                REQUIRE(soc_cmp(c.max_soc() - c.consumed_soc(e), OP::GREATER_EQUAL, c.min_soc()));
            }
            THEN("Can't traverse with SoC == consumed_energy") {
                auto minimumSoC = c.consumed_soc(e);
                REQUIRE(c.can_traverse(e, minimumSoC) == false);
                REQUIRE(soc_cmp(minimumSoC - c.consumed_soc(e), OP::SMALLER, c.min_soc()));
            }
            std::vector<double> can_socs = { 0.25, 0.2577, 0.266, 0.299, 0.31, 0.3231, 0.455, 0.564, 0.796, 0.95, 1.0 };
            for (auto &soc : can_socs) {
                WHEN("SoC = " << soc * 100.0 << "%") {
                    THEN("Can traverse the edge") {
                        REQUIRE(c.can_traverse(e, soc) == true);
                        REQUIRE(soc_cmp(soc, OP::GREATER_EQUAL, c.consumed_soc(e)));
                        REQUIRE(soc_cmp(soc - c.consumed_soc(e), OP::GREATER_EQUAL, c.min_soc()));
                    }
                }
            }

            std::vector<double> too_low_to_traverse_socs = { 0.09, 0.1, 0.112, 0.1224, 0.1443 };
            for (auto &soc : too_low_to_traverse_socs) {
                WHEN("SoC = " << soc * 100.0 << "%") {
                    THEN("Can't traverse the edge") {
                        REQUIRE(c.can_traverse(e, soc) == false);
                        REQUIRE(soc_cmp(soc, OP::SMALLER, c.consumed_soc(e)));
                        REQUIRE(soc_cmp(soc - c.consumed_soc(e), OP::SMALLER, c.min_soc()));
                    }
                }
            }

            std::vector<double> good_to_traverse_but_less_than_min_soc = { 0.192, 0.223, 0.2344, 0.2435 };
            for (auto &soc : good_to_traverse_but_less_than_min_soc) {
                WHEN("SoC = " << soc * 100.0 << "%") {
                    THEN("Can't traverse the edge") {
                        REQUIRE(c.can_traverse(e, soc) == false);
                        REQUIRE(soc_cmp(soc, OP::GREATER_EQUAL, c.consumed_soc(e)));
                        REQUIRE(soc_cmp(soc - c.consumed_soc(e), OP::SMALLER, c.min_soc()));
                    }
                }
            }
        }
    }
}

TEST_CASE("Charge time calculations", "[CAR, ROUTER]") {
    GIVEN("Tesla Model 3 as a car") {
        Car c = Car();
        Node start = Node(12);
        Node end = Node(77);
        GIVEN("Edge which can be traversed with given SoC") {
            Edge e = Edge(&start, &end, 50, 120);
            std::vector<double> initial_socs = { 0.445, 0.6, 0.6512, 0.5521, 0.787, 0.8562, 0.84123 };
            std::sort(initial_socs.begin(), initial_socs.end());
            Time previous_charge_time_to_max = UINT32_MAX;
            for (auto &soc : initial_socs) {
                WHEN("Initial SoC = " << soc * 100.0 << "%") {
                    THEN("Charge time = 0") {
                        REQUIRE(c.can_traverse(e, soc) == true);
                        REQUIRE(c.can_traverse_with_max_soc(e) == true);
                        REQUIRE(c.get_charge_time_to_traverse(e, soc) == 0);
                    }
                    THEN("Time to charge to max is lower than the predecessors") {
                        auto time_to_max = c.get_charge_time_to_max(e, soc);
                        REQUIRE(previous_charge_time_to_max >= time_to_max);
                        previous_charge_time_to_max = time_to_max;
                    }
                }
            }
            WHEN("Initial SoC = SoC MAX") {
                THEN("Time to charge to max = 0") {
                    REQUIRE(c.get_charge_time_to_max(e, c.max_soc()) == 0);
                    REQUIRE(c.get_charge_time_to_traverse(e, c.max_soc()) == 0);
                }
            }
        }
    }
}
