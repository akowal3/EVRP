//
// Created by akowal3 on 07/05/2021.
//

#ifndef EVRP_ROUTER_HPP
#define EVRP_ROUTER_HPP

#include <car.h>
#include <label.hpp>
#include <node.h>
#include <utils.hpp>

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef struct {
    std::vector<const Node *> nodes;
    std::vector<const Edge *> arcs;
    std::vector<double> socs_in;
    std::vector<double> socs_out;
    std::vector<label_type> charges;
    std::vector<Time> charge_times;
    Time total_time;
    Time charge_time;
} RouterResult;

extern inline std::ostream &operator<<(std::ostream &os, const label_type &type);


class Router {
private:
    std::vector<Node> nodes;                              // NodeID, node
    std::unordered_map<unsigned, std::vector<Edge>> edges;// NodeID, edge list (adjacency list)
    static RouterResult build_result(const std::unordered_map<unsigned int, Label> &spt, const Car &c, unsigned int sourceID, unsigned int destinationID);

public:
    RouterResult route(unsigned int sourceID, unsigned int destinationID, const Car &c) const;
    Router(int charger_count, const std::vector<BuildingEdge> &edges);
    static void printSPT(const RouterResult &res);
};


#endif//EVRP_ROUTER_HPP
