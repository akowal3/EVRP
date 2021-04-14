#include <car.h>
#include <edge.h>
#include <graph.h>
#include <iostream>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/geo_position_to_node.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/osm_simple.h>
#include <routingkit/timer.h>
#include <string>
#include <vector>

using namespace RoutingKit;

//inline int main_sample() {
//    // Load a car routing graph from OpenStreetMap-based data
//    //    auto graph = simple_load_osm_car_routing_graph_from_pbf(
//    //            "../OSM/opolskie-latest.osm.pbf");
//    //    auto tail = invert_inverse_vector(graph.first_out);
//
//
//    auto tail = std::vector<unsigned>{0, 0, 1, 1, 2, 3};
//    auto head = std::vector<unsigned>{1, 2, 3, 4, 3, 4};
//    auto distance = std::vector<unsigned>{10, 7, 5, 12, 11, 2};
//    unsigned node_count = 5;
//
//    // Build the shortest path index
//    auto ch = ContractionHierarchy::build(node_count, tail, head, weight);
//
//    ContractionHierarchyQuery query(ch);
//
//    unsigned source_node = 0;
//    unsigned target_node = 3;
//
//    query.reset().add_source(source_node).add_target(target_node).run();
//    unsigned distance = query.get_distance();
//
//    std::cout << "Distance from " << source_node << " to " << target_node << " = " << distance << std::endl;
//
//    std::vector<unsigned> nodes = query.get_node_path();
//
//    std::cout << "Path: ";
//    for (auto node : nodes) {
//        std::cout << node << ", ";
//    }
//    std::cout << std::endl;
//    std::vector<unsigned> arcs = query.get_arc_path();
//
//    auto x = inf_weight;
//}

void graph_build() {
    //    auto tail = std::vector<unsigned>{0, 0, 1, 1, 2, 3};
    //    auto head = std::vector<unsigned>{1, 2, 3, 4, 3, 4};
    //    auto distance = std::vector<unsigned>{10, 7, 5, 12, 11, 2};
    unsigned node_count = 5;

    std::vector<Edge> edges = {Edge(0, 1, 10), Edge(0, 2, 7), Edge(1, 3, 5), Edge(1, 4, 12), Edge(2, 3, 11), Edge(3, 4, 2)};
    Graph g = Graph(node_count, edges);
    Car c = Car();

    for (auto h : g.head) {
        std::cout << h << ", ";
    }
    std::cout << std::endl;

    for (auto t : g.tail) {
        std::cout << t << ", ";
    }
    std::cout << std::endl;

    std::vector<unsigned> travel_time((g.size()));
    //    for (auto w : *g.weights) {
    //        travel_time.push_back(w(c));// can be an array of const size
    //        std::cout << w(c) << ", ";
    //    }

    for (int i = 0; i < g.size(); ++i) {
        travel_time[i] = g.eval(i, c);
        std::cout << travel_time[i] << ", ";
    }
    std::cout << std::endl;

    // Build the shortest path index
    auto ch = ContractionHierarchy::build(node_count, g.tail, g.head, travel_time);

    ContractionHierarchyQuery query(ch);

    unsigned source_node = 0;
    unsigned target_node = 4;

    std::cout << "Give source and target nodes" << std::endl;
    std::cin >> source_node >> target_node;


    query.reset().add_source(source_node).add_target(target_node).run();
    unsigned distance = query.get_distance();

    std::cout << "Distance from " << source_node << " to " << target_node << " = " << distance << std::endl;

    std::vector<unsigned> nodes = query.get_node_path();

    std::cout << "Path: ";
    for (auto node : nodes) {
        std::cout << node << ", ";
    }
}

int main() {
    graph_build();
    return 0;
}