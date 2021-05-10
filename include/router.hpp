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
    std::vector<Node> nodes;                              // NodeID, node
    std::unordered_map<unsigned, std::vector<Edge>> edges;// NodeID, edge list (adjacency list)
public:
    std::unordered_map<unsigned int, Label> route(unsigned int sourceID, unsigned int destinationID, const Car &c) const;
    Router(int charger_count, const std::vector<BuildingEdge> &edges);
    static void printSPT(const std::unordered_map<unsigned int, Label> &spt, unsigned int sourceID, unsigned int destinationID);
};


#endif//EVRP_ROUTER_HPP
