//
// Created by akowal3 on 07/05/2021.
//

#include <algorithm>
#include <cassert>
#include <graph.hpp>
#include <iostream>
#include <router.hpp>

using PriorityQueue = std::priority_queue<Label, std::vector<Label>, std::greater<>>;

Router::Router(int charger_count, const std::vector<BuildingEdge> &edges) {
    this->temp_nodes = {};

    this->nodes.reserve(charger_count);
    for (int id = 0; id < charger_count; id++) {
        this->nodes.emplace_back(Node(id));
    }

    this->edges = std::unordered_map<unsigned int, std::vector<Edge>>{};
    for (const BuildingEdge &e : edges) {
        for (auto speed_modifier : Graph::SPEED_STEPS) {
            this->edges[e.from].push_back(Edge(&this->nodes.at(e.from), &this->nodes.at(e.to), e.distance, e.max_speed * speed_modifier));
        }
    }
}

Router::Router(std::vector<Node> nodes, const std::vector<BuildingEdge> &edges) :
    nodes(std::move(nodes)) {
    this->temp_nodes = {};

    for (const BuildingEdge &e : edges) {
        for (auto speed_modifier : Graph::SPEED_STEPS) {
            this->edges[e.from].push_back(Edge(&this->nodes.at(e.from), &this->nodes.at(e.to), e.distance, e.max_speed * speed_modifier));
        }
    }
}

void Router::add_node(Node n, const std::vector<BuildingEdge> &new_edges) {

    this->temp_nodes.insert({ n.id(), n });

    for (const BuildingEdge &e : new_edges) {
        for (auto speed_modifier : Graph::SPEED_STEPS) {

            const Node *source = (this->temp_nodes.find(e.from) != this->temp_nodes.end()) ? &this->temp_nodes.at(e.from) : &this->nodes.at(e.from);
            const Node *destination = (this->temp_nodes.find(e.to) != this->temp_nodes.end()) ? &this->temp_nodes.at(e.to) : &this->nodes.at(e.to);

            this->edges[e.from].push_back(Edge(source, destination, e.distance, e.max_speed * speed_modifier));
        }
    }
}

// function used to remove the temporary target/destination nodes added at query time
void Router::remove_node_by_id(unsigned nodeID) {
    if (this->temp_nodes.find(nodeID) != this->temp_nodes.end()) {
        this->edges[nodeID].clear();// clear vector of connections from nodeID
        this->edges.erase(nodeID);  // remove entry in the map

        for (auto &m : this->edges) {
            // remove all the edges which have the nodeID as the destination
            m.second.erase(
                    std::remove_if(
                            m.second.begin(), m.second.end(),
                            [nodeID](const Edge &e) {
                                return e.headID() == nodeID;
                            }),
                    m.second.end());
        }

        this->temp_nodes.erase(nodeID);
    }
}

void Router::remove_node(const Node &node) {
    remove_node_by_id(node.id());
}

RouterResult Router::route(unsigned int sourceID, unsigned int destinationID, const Car &c) const {
    auto spt = route_internal(sourceID, destinationID, c);
    return build_result(spt, c, sourceID, destinationID);
}

RouterResult Router::build_result(const std::unordered_map<unsigned int, Label> &spt, const Car &c, unsigned sourceID, unsigned destinationID) {
    // possibly could be made more efficiently without vectors
    RouterResult res = RouterResult();
    Label current = spt.at(destinationID);
    res.total_time = current.get_total_time();


    while (current.get_nodeID() != sourceID) {
        auto consumed_soc = c.consumed_soc(*current.edge);
        res.socs_in.push_back(current.soc());
        res.socs_out.push_back(current.soc() + consumed_soc);
        res.charge_times.push_back(current.get_charge_time());
        res.arcs.push_back(current.edge);
        res.nodes.push_back(current.edge->destinationCharger());
        res.charges.push_back(current.type);
        res.consumed_energy.push_back(c.consumed_power(*current.edge));
        res.consumed_soc.push_back(consumed_soc);
        current = spt.at(current.get_parentID());
    }

    res.nodes.push_back(res.arcs.back()->sourceCharger());
    res.socs_in.push_back(c.initial_soc());// ensure that source point has values for both socs_in and socs_out

    for (auto t : res.charge_times) {
        res.charge_time += t;
    }

    std::reverse(res.socs_out.begin(), res.socs_out.end());
    std::reverse(res.socs_in.begin(), res.socs_in.end());
    std::reverse(res.charge_times.begin(), res.charge_times.end());
    std::reverse(res.arcs.begin(), res.arcs.end());
    std::reverse(res.nodes.begin(), res.nodes.end());
    std::reverse(res.charges.begin(), res.charges.end());
    std::reverse(res.consumed_energy.begin(), res.consumed_energy.end());
    std::reverse(res.consumed_soc.begin(), res.consumed_soc.end());

    res.socs_out.push_back(res.socs_in.back());// ensure that destination point has values for both socs_in and socs_out

    return res;
}
std::unordered_map<unsigned int, Label> Router::route_internal(unsigned int sourceID, unsigned int destinationID, const Car &c) const {
    // adapted from https://github.com/keyan/ev_routing_engine
    // All labels in the vector are non-dominating in respect to total_time and soc_left.
    // That is, all labels for a node are Pareto optimal!
    std::unordered_map<unsigned, std::vector<Label>> label_map;
    // Once a NodeID is added to the spt, we know the best Label to use to get to it.
    std::unordered_map<unsigned, Label> shortest_path_tree;

    PriorityQueue label_queue;

    // lazy deletes as std::priority_queue doesn't allow for arbitrary deletes
    std::unordered_set<unsigned> deleted;

    label_queue.emplace(Label(sourceID, c.initial_soc()));

    while (!label_queue.empty()) {
        Label current = label_queue.top();
        label_queue.pop();

        unsigned currentID = current.get_nodeID();

        if (deleted.find(current.get_labelID()) != deleted.end() ||
            shortest_path_tree.find(currentID) != shortest_path_tree.end()) {
            continue;
        }

        //        shortest_path_tree.emplace(currentID, current);
        shortest_path_tree.insert({ currentID, current });

        // Search is done.
        if (currentID == destinationID) {
            break;
            // clear label counter
        }

        //         Update weights for all neighbors not in the spt.
        //         This is the main departure from standard dijkstra's. Instead of relaxing edges between
        //         neighbors, we construct "labels" up to 3 per neighbor, and try to merge them into the
        //         LabelMap. Any non-dominated labels are also added to the priority queue.
        if (auto connections = this->edges.find(currentID); connections != this->edges.end()) {
            // if currentID has no connections coming ouf of it then we drop it and not enter this for loop
            for (auto &edge : connections->second) {//edges.at(currentID)

                unsigned connectionID = edge.headID();

                if (shortest_path_tree.count(connectionID) == 1 || connectionID == sourceID) {
                    continue;
                }

                Time travel_time = edge.get_travel_time();

                std::vector<Label> labels;

                if (connectionID != destinationID) {
                    // 1. Go to neighbor without any charging, if possible.
                    if (c.can_traverse(edge, current.soc())) {
                        labels.emplace_back(Label(label_type::NO_CHARGE, connectionID, currentID, current.get_total_time() + travel_time,
                                                  0, c.soc_left(edge, current.soc()), &edge));
                    }
                    // 2. Do a full recharge, if needed.
                    if (soc_cmp(current.soc(), OP::SMALLER, c.max_soc()) &&
                        c.can_traverse_with_max_soc(edge) &&
                        c.can_charge(edge)) {

                        Time full_charge_time = c.get_charge_time_to_max(edge, current.soc());
                        labels.emplace_back(Label(label_type::CHARGE_MAXIMUM, connectionID, currentID,
                                                  current.get_total_time() + travel_time + full_charge_time,
                                                  full_charge_time, c.soc_left_from_max_soc(edge), &edge));
                    }

                    // 4. Charge to 70%
                    if (soc_cmp(current.soc(), OP::SMALLER, 0.6) &&
                        c.can_traverse(edge, 0.6) && c.can_charge(edge)) {
                        Time partial_charge_time = c.get_charge_time(edge.sourceCharger(), current.soc(), 0.6);
                        labels.emplace_back(Label(label_type::CHARGE_70, connectionID, currentID,
                                                  current.get_total_time() + travel_time + partial_charge_time,
                                                  partial_charge_time, c.soc_left(edge, 0.6), &edge));
                    }

                    // 4. Charge to 80%
                    if (soc_cmp(current.soc(), OP::SMALLER, 0.8) &&
                        c.can_traverse(edge, 0.8) && c.can_charge(edge)) {
                        Time partial_charge_time = c.get_charge_time(edge.sourceCharger(), current.soc(), 0.8);
                        labels.emplace_back(Label(label_type::CHARGE_80, connectionID, currentID,
                                                  current.get_total_time() + travel_time + partial_charge_time,
                                                  partial_charge_time, c.soc_left(edge, 0.8), &edge));
                    }

                    // 3. Only charge enough to get to neighbor.
                    if (soc_cmp(current.soc(), OP::SMALLER, c.max_soc()) &&
                        !c.can_traverse(edge, current.soc()) &&
                        c.can_traverse_with_max_soc(edge) &&
                        c.can_charge(edge)) {
                        Time partial_charge_time = c.get_charge_time_to_traverse(edge, current.soc());
                        labels.emplace_back(Label(label_type::CHARGE_MINIMUM, connectionID, currentID,
                                                  current.get_total_time() + travel_time + partial_charge_time,
                                                  partial_charge_time, c.min_soc(), &edge));
                    }


                } else {
                    if (c.can_traverse_final(edge, current.soc())) {
                        labels.emplace_back(Label(label_type::NO_CHARGE, connectionID, currentID, current.get_total_time() + travel_time,
                                                  0, c.soc_left(edge, current.soc()), &edge));
                    }

                    if (soc_cmp(current.soc(), OP::SMALLER, c.max_soc()) &&
                        !c.can_traverse_final(edge, current.soc()) &&
                        c.can_traverse_with_max_soc_final(edge) &&
                        c.can_charge(edge)) {
                        Time partial_charge_time = c.get_charge_time_to_traverse_final(edge, current.soc());
                        labels.emplace_back(Label(label_type::CHARGE_MINIMUM, connectionID, currentID,
                                                  current.get_total_time() + travel_time + partial_charge_time,
                                                  partial_charge_time, c.min_soc_final(), &edge));
                    }
                }


                // Update this nodes label bag. This is similar to "relaxing" edges in standard Dijkstra's.
                auto search = label_map.find(connectionID);
                if (search == label_map.end()) {
                    // No labels exist to dominate these ones, so add them all.
                    for (auto &label : labels) {
                        label_queue.push(label);
                    }
                    label_map[connectionID] = std::move(labels);
                } else {
                    std::vector<Label> &bag = search->second;

                    for (auto &label : labels) {
                        auto search = std::find_if(bag.begin(), bag.end(),
                                                   [&](const Label &other) { return other.dominates(label); });
                        // This label is dominated, it can be ignored.
                        if (search != bag.end()) {
                            continue;
                        }

                        // Does this label dominate anything in the bag already? If so, remove those labels.
                        auto d_it = std::partition(bag.begin(), bag.end(),
                                                   [&](const Label &other) { return !label.dominates(other); });
                        for (auto it = d_it; it != bag.end(); ++it) {
                            deleted.insert((*it).get_labelID());
                        }
                        bag.erase(d_it, bag.end());
                        bag.push_back(label);
                        label_queue.push(label);
                    }
                }
            }
        }
    }

    return shortest_path_tree;
}

bool operator==(const Router &left, const Router &right) {

    bool nodes_equal = std::equal(left.nodes.begin(), left.nodes.end(), right.nodes.begin());

    bool edges_equal = std::all_of(left.edges.begin(), left.edges.end(), [&](const auto &p) {
                           if (auto right_edges = right.edges.find(p.first); right_edges != right.edges.end()) {
                               return std::equal(p.second.begin(), p.second.end(), right_edges->second.begin());
                           }
                           return false;
                       }) &&
                       std::all_of(right.edges.begin(), right.edges.end(), [&](const auto &p) {
                           if (auto left_edges = left.edges.find(p.first); left_edges != left.edges.end()) {
                               return std::equal(p.second.begin(), p.second.end(), left_edges->second.begin());
                           }
                           return false;
                       });

    bool temp_nodes_equal = std::all_of(left.temp_nodes.begin(), left.temp_nodes.end(), [&](const auto &p) {
                                if (auto n = right.temp_nodes.find(p.first); n != right.temp_nodes.end()) {
                                    return p.second == n->second;
                                }
                                return false;
                            }) &&
                            std::all_of(right.temp_nodes.begin(), right.temp_nodes.end(), [&](const auto &p) {
                                if (auto n = left.temp_nodes.find(p.first); n != left.temp_nodes.end()) {
                                    return p.second == n->second;
                                }
                                return false;
                            });

    return nodes_equal && edges_equal && temp_nodes_equal;
}

bool operator!=(const Router &left, const Router &right) {
    return !operator==(left, right);
}


std::ostream &operator<<(std::ostream &os, const RouterResult &res) {
    for (int i = 0; i < res.nodes.size(); i++) {
        // print node
        //        if (i == 0) {
        //            os << res.nodes[i]->id() << ": " << res.socs_out[i] << std::endl;
        //        } else if (i == res.nodes.size() - 1) {
        //            os << res.nodes[i]->id() << ": " << res.socs_in[i - 1] << std::endl;
        //        } else {
        //            os << res.nodes[i]->id() << ": " << res.socs_in[i - 1] << " -> " << res.socs_out[i] << " in " << res.charge_times[i] / 3600.0 << " hours (" << res.charges[i] << ")" << std::endl;
        //        }

        if (soc_cmp(res.socs_in[i], OP::EQUAL, res.socs_out[i])) {
            os << res.nodes[i]->id() << ": " << res.socs_in[i] << std::endl;
        } else {
            os << res.nodes[i]->id() << ": " << res.socs_in[i] << " -> " << res.socs_out[i] << " in " << res.charge_times[i] / 3600.0 << " hours (" << res.charges[i] << ")" << std::endl;
        }

        // print path
        if (i != res.nodes.size() - 1) {
            auto edge = res.arcs[i];
            os << "| " << edge->get_distance() << " km @ " << edge->get_speed() << " km/h in " << edge->get_travel_time() / 3600.0 << " hours. Consumed: " << 100 * res.consumed_soc[i] << "% (" << res.consumed_energy[i] << " kWh)" << std::endl;
        }
    }

    os << "Total journey time: " << res.total_time / 3600.0 << " hours, including " << res.charge_time / 3600.0 << " hours of charging" << std::endl;
    return os;
}
