#include <car.hpp>
#include <edge.hpp>
#include <graph.hpp>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/geo_position_to_node.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/osm_simple.h>
#include <routingkit/timer.h>

#include <iostream>
#include <string>
#include <vector>

using namespace RoutingKit;

//inline int main_sample() {
//    // Load a car routing graph from OpenStreetMap-based data
//    //    auto graph = simple_load_osm_car_routing_graph_from_pbf(
//    //            "../OSM/opolskie-latest.osm.pbf");
//    //    auto tailID = invert_inverse_vector(graph.first_out);
//
//
//    auto tailID = std::vector<unsigned>{0, 0, 1, 1, 2, 3};
//    auto head = std::vector<unsigned>{1, 2, 3, 4, 3, 4};
//    auto distance = std::vector<unsigned>{10, 7, 5, 12, 11, 2};
//    unsigned node_size = 5;
//
//    // Build the shortest path index
//    auto ch = ContractionHierarchy::build(node_size, tailID, head, weight);
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

//void graph_build() {
//    //    auto tailID = std::vector<unsigned>{0, 0, 1, 1, 2, 3};
//    //    auto head = std::vector<unsigned>{1, 2, 3, 4, 3, 4};
//    //    auto distance = std::vector<unsigned>{10, 7, 5, 12, 11, 2};
//    unsigned node_size = 5;
//    std::vector<Edge> edges = {Edge(0, 1, 100, 80), Edge(0, 2, 70, 120),
//                               Edge(1, 3, 50, 100), Edge(1, 4, 120, 100),
//                               Edge(2, 3, 110, 60), Edge(3, 4, 20, 120)};
//    Graph g = Graph(node_size, edges);
//
//    for (auto h : g.head) {
//        std::cout << h << ", ";
//    }
//    std::cout << std::endl;
//
//    for (auto t : g.tailID) {
//        std::cout << t << ", ";
//    }
//    std::cout << std::endl;
//
//    std::vector<unsigned> total_time((g.size()));
//    //    for (auto w : *g.weights) {
//    //        total_time.push_back(w(c));// can be an array of const size
//    //        std::cout << w(c) << ", ";
//    //    }
//
//    unsigned source_node = 0;
//    unsigned target_node = 4;
//
//    std::cout << "Give source and target nodes" << std::endl;
//    std::cin >> source_node >> target_node;
//
//    Car c = Car();
//
//    for (int i = 0; i < g.size(); ++i) {
//        total_time[i] = g.eval(i, c);
//        std::cout << total_time[i] << ", ";
//    }
//    std::cout << std::endl;
//
//    // Build the shortest path index
//    auto ch = ContractionHierarchy::build(node_size, g.tailID, g.head, total_time);
//
//    ContractionHierarchyQuery query(ch);
//
//
//    query.reset().add_source(source_node).add_target(target_node).run();
//    unsigned distance = query.get_distance();
//
//    if (distance != inf_weight) {
//        std::cout << "Distance from " << source_node << " to " << target_node << " = " << distance << std::endl;
//
//        std::vector<unsigned> nodes = query.get_node_path();
//
//        std::cout << "Path: ";
//        for (auto node : nodes) {
//            std::cout << node << ", ";
//        }
//    } else {
//        std::cout << "No path from " << source_node << " to " << target_node << std::endl;
//    }
//}

int main() {
    //    graph_build();
    return 0;
}