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

TEST_CASE("Stress test with node addition", "[ROUTER]") {
    typedef struct {
        unsigned from;
        unsigned to;
        RouterResult res;
    } TestResult;

    //    srandom(std::time(nullptr));

    int node_count = 50;
    std::vector<Node> nodes;
    nodes.reserve(node_count);

    for (int i = 0; i < node_count; i++) {
        charger_type type = (random() % 2) ? charger_type::FAST_175KW : charger_type::SLOW_50KW;
        nodes.emplace_back(Node(i, 1.0, type));
    }

    std::vector<BuildingEdge> edges;
    int edge_count = 3000;
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


    unsigned reps = 20;
    std::vector<TestResult> results;
    results.reserve(reps);

    for (int i = 0; i < reps; i++) {
        unsigned from = random() % node_count;
        unsigned to = random() % node_count;
        while (from == to) to = random() % node_count;
        assert(from != to);
        results.emplace_back(TestResult{ from, to, r.route(from, to, c) });
    }

    auto newNodeID = node_count;
    Node n = Node(newNodeID);

    std::vector<BuildingEdge> new_edges;
    int newEdgeCount = 100;
    new_edges.reserve(newEdgeCount * 4);

    for (int i = 0; i < newEdgeCount; ++i) {
        unsigned from = random() % (node_count + 1);
        unsigned to = random() % (node_count + 1);
        while (from == to || to == newNodeID || from == newNodeID) to = random() % (node_count + 1);
        assert(from != to);
        assert(0 <= from && from <= newNodeID);
        assert(0 <= to && to <= newNodeID);

        unsigned distance = random() % 600 + 80;
        unsigned speed = random() % 200 + 30;

        new_edges.emplace_back(BuildingEdge(newNodeID, to, distance, speed));
        new_edges.emplace_back(BuildingEdge(newNodeID, from, distance, speed));
        new_edges.emplace_back(BuildingEdge(from, newNodeID, distance, speed));
        new_edges.emplace_back(BuildingEdge(to, newNodeID, distance, speed));
    }

    r.add_node(n, new_edges);

    for (auto &prev : results) {
        auto res = r.route(prev.from, prev.to, c);
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

    r.pop_node();

    r.pop_node();

    bool equal = r2 == r;

    REQUIRE(equal);

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
    //
    //    std::cout << r.route(src.id(), 5, c) << std::endl;
}