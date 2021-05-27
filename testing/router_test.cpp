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

TEST_CASE("Router ==operator", "[ROUTER]") {

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

    Router r = Router(node_count, edges);
    Router r2 = Router(node_count, edges);

    REQUIRE(r == r2);
    REQUIRE(r2 == r);
    REQUIRE(!(r2 != r));
    REQUIRE(!(r != r2));

    r = Router(nodes, edges);
    r2 = Router(nodes, edges);

    REQUIRE(r == r2);
    REQUIRE(r2 == r);
    REQUIRE(!(r2 != r));
    REQUIRE(!(r != r2));

    r.add_node(Node(node_count), {
                                         BuildingEdge(node_count, 8, 100, 70),
                                 });
    REQUIRE(r != r2);
    REQUIRE(r2 != r);
    REQUIRE(!(r2 == r));
    REQUIRE(!(r == r2));

    r.remove_node_by_id(node_count);
    REQUIRE(r == r2);
    REQUIRE(r2 == r);
    REQUIRE(!(r2 != r));
    REQUIRE(!(r != r2));
}


TEST_CASE("Stress test", "[ROUTER]") {
    //    srandom(std::time(nullptr));

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
    Car c = Car::TeslaModel3(0.7, 0.15, 0.9, 0.4);

    for (int i = 0; i < 20; i++) {
        unsigned from = random() % node_count;
        unsigned to = random() % node_count;
        while (from == to) to = random() % node_count;
        assert(from != to);

        r.route_internal(from, to, c);
        std::cout << "Internal was fine" << std::endl;

        std::cout << r.route(from, to, c) << std::endl;
    }
}

TEST_CASE("Node addition", "[ROUTER]") {
    std::vector<BuildingEdge> edges = {
        BuildingEdge(0, 1, 100, 120),
        BuildingEdge(1, 2, 25, 11),
        BuildingEdge(2, 3, 110, 140),
        BuildingEdge(3, 4, 44, 133),
        //        BuildingEdge(4, 5, 44, 133),
        BuildingEdge(0, 5, 70, 100),
        BuildingEdge(5, 6, 70, 100),
        BuildingEdge(6, 7, 150, 100),
        BuildingEdge(7, 4, 100, 100),
        BuildingEdge(6, 2, 100, 70),
        BuildingEdge(6, 3, 100, 70),

    };
    int node_count = 8;
    Router r = Router(node_count, edges);
    Router r2 = Router(node_count, edges);

    REQUIRE(r == r2);

    Car c = Car(0.5);
    std::cout << r.route(0, 7, c) << std::endl;

    Node dst = Node(8);

    r.add_node(dst, {
                            BuildingEdge(6, 8, 100, 70),
                    });

    std::cout << r.route(0, dst.id(), c) << std::endl;

    r2.route(0, 6, c);
    REQUIRE(r != r2);

    Node src = Node(9);

    r.add_node(src, {
                            BuildingEdge(9, 0, 20, 50),
                            BuildingEdge(9, 5, 20, 50),
                            BuildingEdge(9, 4, 100, 70),
                            BuildingEdge(9, 6, 100, 70),
                            BuildingEdge(9, 8, 150, 70),
                    });

    std::cout << "node added" << std::endl;

    std::cout << r.route(src.id(), dst.id(), c) << std::endl;

    r.remove_node(src);

    r.remove_node(dst);

    REQUIRE(r2 == r);

    r.add_node(dst, {
                            BuildingEdge(6, 8, 100, 70),
                    });

    r.add_node(src, {
                            BuildingEdge(9, 0, 20, 50),
                            BuildingEdge(9, 5, 20, 50),
                            BuildingEdge(9, 4, 100, 70),
                            BuildingEdge(9, 6, 100, 70),
                    });

    std::cout << r.route(src.id(), dst.id(), c) << std::endl;

    r.remove_node(src);

    r.remove_node(dst);

    REQUIRE(r2 == r);
    //
    //    std::cout << r.route(src.id(), 5, c) << std::endl;
}