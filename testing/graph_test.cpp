//
// Created by akowal3 on 29/04/2021.
//

#include <car.h>
#include <edge.h>
#include <graph.h>
#include <routingkit/contraction_hierarchy.h>

#include <catch.hpp>
#include <graph_cases.hpp>
#include <iomanip>
#include <iostream>
#include <testcase.hpp>
#include <unordered_set>

using namespace RoutingKit;


void MapPrint(const std::unordered_map<unsigned, std::unordered_map<unsigned, std::unordered_map<unsigned, unsigned>>> &NodeMap) {
    std::cout << "{" << std::endl;
    for (auto &[from, to_map] : NodeMap) {
        std::cout << "\tfrom: " << from << ": {" << std::endl;
        for (auto &[to, travel_map] : to_map) {
            std::cout << "\t\tto: " << to << ": {" << std::endl;
            for (auto &[travel_time, soc_count] : travel_map) {
                std::cout << "\t\t\ttime:" << travel_time << ", soc: " << soc_count << std::endl;
            }
            std::cout << "\t\t}" << std::endl;
        }
        std::cout << "\t}" << std::endl;
    }
    std::cout << "}" << std::endl;
}

void PathPrint(const Graph &g, const std::vector<unsigned> &arc_path) {
    unsigned total_time = 0;
    unsigned only_travel = 0;
    std::cout << "Path goes through: " << std::endl;
    Car c = Car();
    for (auto it = arc_path.begin(); it != arc_path.end(); ++it) {
        auto edge = g.lookup_edges->at(*it);
        unsigned tailID = edge.tail();

        only_travel += edge.get_travel_time();

        std::cout << Graph::originalID(tailID) << "(" << g.lookup_nodes->at(tailID).soc() * 100 << "%) --@" << edge.get_speed() << "km/h--> ";

        if (it == arc_path.end() - 1) {
            unsigned headID = edge.head();
            std::cout << Graph::originalID(headID) << "(" << g.lookup_nodes->at(headID).soc() * 100 << "%)" << std::endl;
        }
    }
    std::cout << "Travel time (no charging) was " << only_travel / 3600.0 << " hours" << std::endl;
}

TEST_CASE("Build graph", "[GRAPH]") {

    for (auto &testcase : GraphCases) {
        GIVEN(testcase.description) {
            unsigned node_count = testcase.node_count;
            std::vector<BuildingEdge> e = testcase.graph;

            Graph g = Graph(node_count, e);

            unsigned graph_node_count = node_count * Graph::CHARGER_STEPS.size();
            unsigned graph_edge_count = Graph::CHARGER_STEPS.size() * Graph::CHARGER_STEPS.size() * e.size() * Graph::SPEED_STEPS.size();


            THEN("Graph has " << graph_node_count << " nodes and " << graph_edge_count << " edges") {
                REQUIRE(g.lookup_nodes->size() == graph_node_count);
                REQUIRE(g.lookup_edges->size() == graph_edge_count);
                REQUIRE(g.lookup_weights->size() == graph_edge_count);
            }

            THEN("There are Graph::CHARGER_STEPS.size() * node_size nodes") {
                for (int i = 0; i < g.lookup_nodes->size(); i++) {
                    double expected_soc = Graph::CHARGER_STEPS[i % Graph::CHARGER_STEPS.size()];
                    WHEN("Node " << Graph::originalID(i) << ", charge_level: " << 100 * expected_soc << "%") {
                        THEN("charge_level matches the charge_level levels in Graph::CHARGER_STEPS") {
                            REQUIRE(g.lookup_nodes->at(i).soc() == expected_soc);
                        }
                    }
                }
            }

            THEN("Edges are connecting same nodes and are growing with speeds and charge steps") {
                std::unordered_map<unsigned, std::unordered_map<unsigned, std::unordered_map<unsigned, unsigned>>> ExpectedGraphNodeMap;// from: {to: {travel_time: soc_count}}

                for (auto edge : e) {
                    for (auto speed_modifier : Graph::SPEED_STEPS) {
                        ExpectedGraphNodeMap[edge.from][edge.to][edge.get_travel_time(speed_modifier)] =
                                Graph::CHARGER_STEPS.size() * Graph::CHARGER_STEPS.size();
                    }
                }

                std::unordered_map<unsigned, std::unordered_map<unsigned, std::unordered_map<unsigned, unsigned>>> GraphNodeMap;// from: {to: {travel_time: soc_count}}
                for (auto ge : *g.lookup_edges) {
                    ++GraphNodeMap[ge.tail() / Graph::CHARGER_STEPS.size()][ge.head() / Graph::CHARGER_STEPS.size()][ge.get_travel_time()];
                }

                REQUIRE(ExpectedGraphNodeMap == GraphNodeMap);
            }
        }
    }
}

TEST_CASE("Connect to RoutingKit", "[GRAPH, RK]") {
    // get complicated graph
    unsigned graph_number = 6;
    unsigned node_count = GraphCases[graph_number].node_count;
    std::vector<BuildingEdge> e = GraphCases[graph_number].graph;
    Graph g = Graph(node_count, e);

    std::vector<unsigned> travel_time(g.edge_size());

    unsigned source_node = 4;
    unsigned target_node = 2;

    Car c = Car();

    for (int i = 0; i < g.edge_size(); ++i) {
        travel_time[i] = g.eval(i, c);
    }

    // Build the shortest path index
    auto ch = ContractionHierarchy::build(g.node_size(), g.tail, g.head, travel_time);

    ContractionHierarchyQuery query(ch);

    std::vector<unsigned> targets = Graph::ID_to_nodes(target_node);

    query.reset().add_source(source_node);

    for (auto &t : Graph::ID_to_nodes(target_node)) {
        query.add_target(t);
    }

    query.run();

    unsigned total_time = query.get_distance();

    REQUIRE(total_time != RoutingKit::inf_weight);

    auto arc_path = query.get_arc_path();

    unsigned total_time_from_edges = 0;
    for (auto &it : arc_path) {
        auto edge = g.lookup_edges->at(it);
        auto edge_time = c.traverse(edge);
        REQUIRE(edge_time != RoutingKit::inf_weight);
        total_time_from_edges += edge_time;
    }

    REQUIRE(total_time_from_edges == total_time);

    std::cout << std::setprecision(3) << "Shortest time to go from " << Graph::originalID(source_node)
              << " to " << target_node << " = " << total_time / 3600.0 << " hours" << std::endl;

    PathPrint(g, arc_path);
}

//TEST_CASE("Different car", "[GRAPH]") {
//    std::vector<Edge> e = { Edge(0, 1, 100, 80), Edge(1, 0, 50, 12) };
//    Graph g = Graph(2, e);
//
//    REQUIRE(g.size() == e.size() * Graph::SPEED_STEPS.size() * Graph::CHARGER_STEPS.size() * Graph::CHARGER_STEPS.size());
//
//    int reps = Graph::SPEED_STEPS.size();
//
//    GIVEN("Tesla Model 3") {
//        Car c = Car();
//        for (int i = 0; i < e.size(); i++) {
//            for (int j = 0; j < reps; j++) {
//                REQUIRE(g.eval(i * reps + j, c) == e[i].traverse(c, Graph::SPEED_STEPS[j]));
//            }
//        }
//    }
//
//    GIVEN("Ideal car") {
//        Car ideal = Car(70.0, 0, 85.0, 0.0, 1.0, 1000.0);
//        for (int i = 0; i < e.size(); i++) {
//            for (int j = 0; j < reps; j++) {
//                auto travel_time = g.eval(i * reps + j, ideal);
//                REQUIRE(travel_time == e[i].traverse(ideal, Graph::SPEED_STEPS[j]));
//                REQUIRE(travel_time == e[i].get_travel_time(Graph::SPEED_STEPS[j]));
//            }
//        }
//    }
//}
//
//
//TEST_CASE("Full run", "[CRP,GRAPH,EDGE,CAR]") {
//    GIVEN("Unidirected graph") {
//        unsigned node_size = 5 * Graph::CHARGER_STEPS.size();
//        std::vector<Edge> edges = {Edge(0, 1, 100, 80), Edge(0, 2, 70, 120),
//                                   Edge(1, 3, 50, 100), Edge(1, 4, 120, 100),
//                                   Edge(2, 3, 110, 60), Edge(3, 4, 20, 120)};
//        Graph g = Graph(node_size, edges);
//
//        std::vector<unsigned> travel_time((g.size()));
//
//        unsigned source_node = 0;
//        unsigned target_node = 4;
//
//        Car c = Car();
//
//        for (int i = 0; i < g.size(); ++i) {
//            travel_time[i] = g.eval(i, c);
//        }
//
//
//        // Build the shortest path index
//        auto ch = ContractionHierarchy::build(node_size, g.tail, g.head, travel_time);
//
//        ContractionHierarchyQuery query(ch);
//
//
//        query.reset().add_source(source_node).add_target(target_node).run();
//        unsigned distance = query.get_distance();
//        auto t = query.get_arc_path();
//
//        std::cout << "Distance = " << distance / 3600.0 << "hours" << std::endl;
//    }
//
//    GIVEN("Bidirectional graph") {
//        unsigned node_size = 5;
//        std::vector<Edge> edges = {Edge(0, 1, 100, 80), Edge(0, 2, 70, 120),
//                                   Edge(1, 3, 50, 100), Edge(1, 4, 120, 100),
//                                   Edge(2, 3, 110, 60), Edge(3, 4, 20, 120),
//                                   Edge(1, 0, 100, 80), Edge(2, 0, 70, 120),
//                                   Edge(3, 1, 50, 100), Edge(4, 1, 120, 100),
//                                   Edge(3, 2, 110, 60), Edge(4, 3, 20, 120)};
//        Graph g = Graph(node_size, edges);
//
//        std::vector<unsigned> travel_time((g.size()));
//
//        unsigned source_node = 0;
//        unsigned target_node = 4;
//
//        Car c = Car();
//
//        for (int i = 0; i < g.size(); ++i) {
//            travel_time[i] = g.eval(i, c);
//        }
//
//
//        // Build the shortest path index
//        auto ch = ContractionHierarchy::build(node_size, g.tail, g.head, travel_time);
//
//        ContractionHierarchyQuery query(ch);
//
//
//        query.reset().add_source(source_node).add_target(target_node).run();
//        unsigned distance = query.get_distance();
//        auto t = query.get_arc_path();
//
//        std::cout << "Distance = " << distance / 3600.0 << "hours" << std::endl;
//    }
//}