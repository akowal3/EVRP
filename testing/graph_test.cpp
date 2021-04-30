//
// Created by akowal3 on 29/04/2021.
//

#include <car.h>
#include <catch.hpp>
#include <edge.h>
#include <fstream>//required for parse_file()
#include <graph.h>
#include <iostream>
#include <routingkit/contraction_hierarchy.h>
#include <sstream>

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

TEST_CASE("Build graph", "[GRAPH]") {

    constexpr unsigned TestCount = 6;
    unsigned TEST_node_count[TestCount] = {2, 2, 2, 3, 3, 5};
    std::vector<BuildingEdge> TEST_edges[TestCount] = {
            {BuildingEdge(0, 1, 100, 80)},
            {BuildingEdge(0, 1, 100, 80),
             BuildingEdge(1, 0, 100, 80)},
            {BuildingEdge(0, 1, 100, 80),
             BuildingEdge(0, 1, 101, 77)},
            {
                    BuildingEdge(0, 1, 10, 120),
                    BuildingEdge(0, 2, 18, 18),
                    BuildingEdge(1, 2, 22, 31),
            },
            {
                    BuildingEdge(0, 1, 100, 120),
                    BuildingEdge(0, 2, 25, 11),
                    BuildingEdge(1, 0, 110, 140),
                    BuildingEdge(1, 2, 44, 133),
                    BuildingEdge(2, 1, 47, 112),
            },
            {BuildingEdge(0, 1, 100, 80), BuildingEdge(0, 2, 70, 120),
             BuildingEdge(1, 3, 50, 100), BuildingEdge(1, 4, 120, 100),
             BuildingEdge(2, 3, 110, 60), BuildingEdge(3, 4, 20, 120),
             BuildingEdge(1, 0, 100, 80), BuildingEdge(2, 0, 70, 120),
             BuildingEdge(3, 1, 50, 100), BuildingEdge(4, 1, 120, 100),
             BuildingEdge(3, 2, 110, 60), BuildingEdge(4, 3, 20, 120)},
    };
    std::string TEST_description[TestCount] = {
            "Graph with single edge",
            "Graph with single bidirectional edge",
            "Graph with 2 same edges",
            "Graph with 3 nodes",
            "Graph with 3 nodes (some bidirectional)",
            "Graph with 5 nodes"};


    for (int testID = 0; testID < TestCount; testID++) {
        GIVEN(TEST_description[testID]) {
            unsigned node_count = TEST_node_count[testID];
            std::vector<BuildingEdge> e = TEST_edges[testID];

            Graph g = Graph(node_count, e);

            THEN("Sizes of each vector correspond to number of soc states and number of different travel speeds") {
                REQUIRE(g.lookup_nodes->size() == node_count * Graph::CHARGER_STEPS.size());
                REQUIRE(g.lookup_edges->size() == Graph::CHARGER_STEPS.size() * Graph::CHARGER_STEPS.size() * e.size() * Graph::SPEED_STEPS.size());
                REQUIRE(g.lookup_weights->size() == g.lookup_edges->size());
            }

            THEN("There are Graph::CHARGER_STEPS.size() * node_count nodes") {
                for (int i = 0; i < g.lookup_nodes->size(); i++) {
                    double expected_soc = Graph::CHARGER_STEPS[i % Graph::CHARGER_STEPS.size()];
                    std::ostringstream desc;
                    desc << "Node " << i / Graph::CHARGER_STEPS.size() << ", SoC: " << 100 * expected_soc << "%";
                    WHEN(desc.str()) {
                        THEN("SoC matches the SoC levels in Graph::CHARGER_STEPS") {
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

//TEST_CASE("Build graph 2", "[GRAPH]") {
//    std::vector<Edge> e = {Edge(0, 1, 100, 100), Edge(1, 0, 50, 50)};
//    Graph g = Graph(2, e);
//    Car c = Car();
//
//    REQUIRE(g.size() == e.size() * Graph::SPEED_STEPS.size() * Graph::CHARGER_STEPS.size() * Graph::CHARGER_STEPS.size());
//
//    int reps = Graph::SPEED_STEPS.size();
//    for (int i = 0; i < e.size(); i++) {
//        for (int j = 0; j < reps; j++) {
//            std::string desc = "i = " + std::to_string(i) + ", j = " + std::to_string(j);
//            WHEN(desc) {
//                REQUIRE(g.eval(i * reps + j, c) == e[i].traverse(c, Graph::SPEED_STEPS[j]));
//            }
//        }
//    }
//}
//
//TEST_CASE("Different car", "[GRAPH]") {
//    std::vector<Edge> e = {Edge(0, 1, 100, 80), Edge(1, 0, 50, 12)};
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
//        unsigned node_count = 5 * Graph::CHARGER_STEPS.size();
//        std::vector<Edge> edges = {Edge(0, 1, 100, 80), Edge(0, 2, 70, 120),
//                                   Edge(1, 3, 50, 100), Edge(1, 4, 120, 100),
//                                   Edge(2, 3, 110, 60), Edge(3, 4, 20, 120)};
//        Graph g = Graph(node_count, edges);
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
//        auto ch = ContractionHierarchy::build(node_count, g.tail, g.head, travel_time);
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
//        unsigned node_count = 5;
//        std::vector<Edge> edges = {Edge(0, 1, 100, 80), Edge(0, 2, 70, 120),
//                                   Edge(1, 3, 50, 100), Edge(1, 4, 120, 100),
//                                   Edge(2, 3, 110, 60), Edge(3, 4, 20, 120),
//                                   Edge(1, 0, 100, 80), Edge(2, 0, 70, 120),
//                                   Edge(3, 1, 50, 100), Edge(4, 1, 120, 100),
//                                   Edge(3, 2, 110, 60), Edge(4, 3, 20, 120)};
//        Graph g = Graph(node_count, edges);
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
//        auto ch = ContractionHierarchy::build(node_count, g.tail, g.head, travel_time);
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