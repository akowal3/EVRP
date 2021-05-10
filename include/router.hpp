//
// Created by akowal3 on 07/05/2021.
//

#ifndef EVRP_ROUTER_HPP
#define EVRP_ROUTER_HPP

#include <car.h>
#include <label.hpp>
#include <node.h>

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Router {
private:
    Router(int charger_count, const std::vector<BuildingEdge> &edges);
    std::vector<Node> nodes;                              // NodeID, node
    std::unordered_map<unsigned, std::vector<Edge>> edges;// NodeID, edge list (adjacency list)
public:
    void route(unsigned int sourceID, unsigned int destinationID, const Car &c) const;
};


#endif//EVRP_ROUTER_HPP
