//
// Created by akowal3 on 29/04/2021.
//

#include <car.h>
#include <catch.hpp>
#include <edge.h>
#include <graph.h>
#include <iostream>
#include <routingkit/contraction_hierarchy.h>
#include <sstream>

using namespace RoutingKit;

TEST_CASE("Build graph", "[GRAPH]"){
    Edge e = Edge(0, 1, 100, 80);
    Graph g = Graph(2, {e});
    Car c = Car();

    for(int i = 0; i < Graph::SPEED_STEPS.size(); i++){
        REQUIRE(g.eval(i,c) == e.get_total_time(c, Graph::SPEED_STEPS[i]));
    }

}

TEST_CASE("Build graph 2","[GRAPH]"){
    std::vector<Edge> e = {Edge(0, 1, 100, 100), Edge(1, 0, 50, 50)};
    Graph g = Graph(2, e);
    Car c = Car();

    REQUIRE(g.size() == e.size() * Graph::SPEED_STEPS.size());

    int reps = Graph::SPEED_STEPS.size();
    for(int i = 0; i < e.size(); i++){
        for(int j = 0; j < reps; j++){
            std::string desc = "i = " + std::to_string(i) + ", j = " + std::to_string(j);
            WHEN(desc){
                REQUIRE(g.eval(i*reps+j,c) == e[i].get_total_time(c, Graph::SPEED_STEPS[j]));
            }
        }
    }

}

TEST_CASE("Different car", "[GRAPH]"){
    std::vector<Edge> e = {Edge(0, 1, 100, 80), Edge(1, 0, 50, 12)};
    Graph g = Graph(2, e);

    REQUIRE(g.size() == e.size() * Graph::SPEED_STEPS.size());

    int reps = Graph::SPEED_STEPS.size();

    GIVEN("Tesla Model 3") {
        Car c = Car();
        for(int i = 0; i < e.size(); i++){
            for(int j = 0; j < reps; j++){
                REQUIRE(g.eval(i*reps+j,c) == e[i].get_total_time(c, Graph::SPEED_STEPS[j]));
            }
        }
    }

    GIVEN("Ideal car") {
        Car ideal = Car(70.0, 0, 85.0, 0.0, 1.0, 1000.0);
        for(int i = 0; i < e.size(); i++){
            for(int j = 0; j < reps; j++){
                auto travel_time = g.eval(i*reps+j, ideal);
                REQUIRE(travel_time == e[i].get_total_time(ideal, Graph::SPEED_STEPS[j]));
                REQUIRE(travel_time == e[i].get_travel_time(Graph::SPEED_STEPS[j]));
            }
        }
    }

}


TEST_CASE("Full run", "[CRP,GRAPH,EDGE,CAR]") {
    GIVEN("Unidirected graph"){
        unsigned node_count = 5;
        std::vector<Edge> edges = {Edge(0, 1, 100, 80), Edge(0, 2, 70, 120),
                                   Edge(1, 3, 50, 100), Edge(1, 4, 120, 100),
                                   Edge(2, 3, 110, 60), Edge(3, 4, 20, 120)};
        Graph g = Graph(node_count, edges);

        std::vector<unsigned> travel_time((g.size()));

        unsigned source_node = 0;
        unsigned target_node = 4;

        Car c = Car();

        for (int i = 0; i < g.size(); ++i) {
            travel_time[i] = g.eval(i, c);
        }


        // Build the shortest path index
        auto ch = ContractionHierarchy::build(node_count, g.tail, g.head, travel_time);

        ContractionHierarchyQuery query(ch);


        query.reset().add_source(source_node).add_target(target_node).run();
        unsigned distance = query.get_distance();
        auto t = query.get_arc_path();

        std::cout << "Distance = " << distance / 3600.0 << "hours" << std::endl;
    }

    GIVEN("Bidirectional graph"){
        unsigned node_count = 5;
        std::vector<Edge> edges = {Edge(0, 1, 100, 80), Edge(0, 2, 70, 120),
                                   Edge(1, 3, 50, 100), Edge(1, 4, 120, 100),
                                   Edge(2, 3, 110, 60), Edge(3, 4, 20, 120),
                                   Edge(1, 0, 100, 80), Edge(2, 0, 70, 120),
                                   Edge(3, 1, 50, 100), Edge(4, 1, 120, 100),
                                   Edge(3, 2, 110, 60), Edge(4, 3, 20, 120)};
        Graph g = Graph(node_count, edges);

        std::vector<unsigned> travel_time((g.size()));

        unsigned source_node = 0;
        unsigned target_node = 4;

        Car c = Car();

        for (int i = 0; i < g.size(); ++i) {
            travel_time[i] = g.eval(i, c);
        }


        // Build the shortest path index
        auto ch = ContractionHierarchy::build(node_count, g.tail, g.head, travel_time);

        ContractionHierarchyQuery query(ch);


        query.reset().add_source(source_node).add_target(target_node).run();
        unsigned distance = query.get_distance();
        auto t = query.get_arc_path();

        std::cout << "Distance = " << distance / 3600.0 << "hours" << std::endl;
    }


}