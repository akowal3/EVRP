//
// Created by akowal3 on 10/05/2021.
//


#include <catch.hpp>
#include <graph_cases.hpp>
#include <iostream>
#include <random>
#include <router.hpp>
#include <sstream>
#include <utils.hpp>

TEST_CASE("Simple router verifier", "[ROUTER]") {
    for (auto &tc : RKCases) {
        GIVEN(tc.description) {
            Router router = Router(tc.node_count, tc.graph);

            unsigned sourceID = tc.path.source_original();
            unsigned destinationID = tc.path.target();
            double initial_soc = tc.path.charge_levels.front();// same as for the graph

            Car c = Car(initial_soc);

            RouterResult ret = router.route(sourceID, destinationID, c);
            //            auto total_time = ret.at(destinationID).get_total_time();
            //

            THEN("Calculated path goes through the correct nodes") {
                REQUIRE(ret.nodes.size() == tc.path.nodes.size());
                for (int i = 0; i < ret.nodes.size(); i++) {
                    std::ostringstream msg;

                    if (i == 0) {
                        msg << ret.nodes[i]->id() << ": " << ret.socs_out[i];
                    } else if (i == ret.nodes.size() - 1) {
                        msg << ret.nodes[i]->id() << ": " << ret.socs_in[i - 1] << std::endl;
                    } else {
                        msg << ret.nodes[i]->id() << ": " << ret.socs_in[i - 1] << " -> " << ret.socs_out[i] << " in " << ret.charge_times[i] / 3600.0 << " hours (" << ret.charges[i] << ")" << std::endl;
                    }

                    WHEN(msg.str()) {
                        REQUIRE(ret.nodes[i]->id() == tc.path.nodes[i]);
                    }
                }
            }

            THEN("Calculated path takes edges with expected speeds") {
                REQUIRE(ret.arcs.size() == tc.path.speeds.size());
                for (int i = 0; i < ret.arcs.size(); i++) {
                    auto edge = ret.arcs[i];
                    auto tailID = edge->tailID();
                    auto headID = edge->headID();
                    WHEN(tailID << " -> " << headID << " | " << edge->get_distance() << " km @ " << edge->get_speed() << " km/h in " << edge->get_travel_time() / 3600.0 << " hours") {
                        REQUIRE(edge->get_speed() == tc.path.speeds[i]);
                        REQUIRE(tailID == tc.path.nodes[i]);
                        if (i == ret.arcs.size() - 1) {
                            // for last edge verify the head to match last destinationCharger
                            REQUIRE(headID == tc.path.nodes.back());
                        }
                    }
                }
            }
            AND_THEN("Route takes " << ret.total_time / 3600.0 << " hours (" << ret.total_time << " seconds). Including " << ret.charge_time / 3600.0 << " hours of charging"){};
        }
    }
}


TEST_CASE("Stress test", "[ROUTER]") {
    srandom(std::time(nullptr));

    int node_count = 1000;
    std::vector<Node> nodes;
    nodes.reserve(node_count);

    for (int i = 0; i < node_count; i++) {
        charger_type type = (random() % 2) ? charger_type::FAST_175KW : charger_type::SLOW_50KW;
        nodes.emplace_back(Node(i, 1.0, type));
    }

    std::vector<BuildingEdge> edges;
    int edge_count = 30000;
    edges.reserve(edge_count);

    for (int i = 0; i < edge_count; ++i) {
        unsigned from = random() % node_count;
        unsigned to = random() % node_count;
        while (from == to) to = random() % node_count;
        assert(from != to);
        unsigned distance = random() % 600 + 80;
        unsigned speed = random() % 200 + 30;
        edges.emplace_back(BuildingEdge(from, to, distance, speed));
    }

    Router r = Router(nodes, edges);
    Car c = Car(0.7);

    unsigned from = random() % node_count;
    unsigned to = random() % node_count;
    while (from == to) to = random() % node_count;
    assert(from != to);

    r.route_internal(from, to, c);

    std::cout << "Internal was fine" << std::endl;

    std::cout << r.route(from, to, c) << std::endl;
}