//
// Created by akowal3 on 07/05/2021.
//

#include <algorithm>
#include <cassert>
#include <graph.hpp>
#include <iostream>
#include <router.hpp>

using MinHeapPriorityQueue =
        std::priority_queue<Label, std::vector<Label>, std::greater<std::vector<Label>::value_type>>;


Router::Router(int charger_count, const std::vector<BuildingEdge> &edges) {
    //    this->nodes = std::vector<Node>{};
    this->nodes.reserve(charger_count);
    for (int id = 0; id < charger_count; id++) {
        this->nodes.emplace_back(Node(id));
    }

    for (const BuildingEdge &e : edges) {
        for (auto speed_modifier : Graph::SPEED_STEPS) {
            this->edges[e.from].emplace_back(Edge(&nodes[e.from], &nodes[e.to], e.distance, e.max_speed * speed_modifier));
        }
        //        this->edges[e.from].emplace_back(Edge(&nodes[e.from], &nodes[e.to], e.distance, e.max_speed));
    }
}

RouterResult Router::route(unsigned int sourceID, unsigned int destinationID, const Car &c) const {
    // All labels in the vector are non-dominating in respect to total_time and soc_left.
    // That is, all labels for a node are Pareto optimal!
    std::unordered_map<unsigned, std::vector<Label>> label_map;
    // Once a NodeID is added to the spt, we know the best Label to use to get to it.
    std::unordered_map<unsigned, Label> shortest_path_tree;

    MinHeapPriorityQueue label_queue;

    // lazy deletes as std::priority_queue doesn't allow for arbitrary deletes
    std::unordered_set<unsigned> deleted;

    label_queue.emplace(Label(sourceID, c.initial_soc()));// soc_left = c.initialSoC() coming from the query

    while (!label_queue.empty()) {
        Label current = label_queue.top();
        label_queue.pop();

        unsigned currentID = current.get_nodeID();

        // std::priority_queue has no decrease-weight operation, instead do a "lazy
        // deletion" by keeping the old node in the pq and just ignoring it when it
        // is eventually popped.
        if (deleted.count(current.get_labelID()) == 1 ||
            shortest_path_tree.count(currentID) == 1) {
            continue;
        }

        shortest_path_tree.emplace(currentID, current);

        // Search is done.
        if (currentID == destinationID) {
            break;
        }

        // Update weights for all neighbors not in the spt.
        // This is the main departure from standard dijkstra's. Instead of relaxing edges between
        // neighbors, we construct "labels" up to 3 per neighbor, and try to merge them into the
        // LabelMap. Any non-dominated labels are also added to the priority queue.
        for (auto &edge : edges.at(currentID)) {
            assert(currentID == edge.tailID());

            unsigned connectionID = edge.headID();
            Time travel_time = edge.get_travel_time();

            if (shortest_path_tree.count(connectionID) == 1) {
                continue;
            }

            // Three possible label cases.
            std::vector<Label> labels;
            // 1. Go to neighbor without any charging, if possible.
            if (c.can_traverse(edge, current.soc())) {
                labels.emplace_back(Label(label_type::NO_CHARGE, connectionID, currentID, current.get_total_time() + travel_time,
                                          0, c.soc_left(edge, current.soc()), &edge));
            }
            // 2. Do a full recharge, if needed.
            if (soc_cmp(current.soc(), OP::SMALLER, c.max_soc()) &&
                c.can_traverse_with_max_soc(edge)) {

                Time full_charge_time = c.get_charge_time_to_max(edge, current.soc());
                labels.emplace_back(Label(label_type::CHARGE_MAXIMUM, connectionID, currentID,
                                          current.get_total_time() + travel_time + full_charge_time,
                                          full_charge_time, c.soc_left_from_max_soc(edge), &edge));
            }


            // 3. Only charge enough to get to neighbor.
            if (soc_cmp(current.soc(), OP::SMALLER, c.max_soc()) &&
                !c.can_traverse(edge, current.soc()) &&
                c.can_traverse_with_max_soc(edge)) {
                Time partial_charge_time = c.get_charge_time_to_traverse(edge, current.soc());
                labels.emplace_back(Label(label_type::CHARGE_MINIMUM, connectionID, currentID,
                                          current.get_total_time() + travel_time + partial_charge_time,
                                          partial_charge_time, c.min_soc(), &edge));
            }

            // 4. Charge to 70%
            if (soc_cmp(current.soc(), OP::SMALLER, 0.7) &&
                c.can_traverse(edge, 0.7)) {
                Time partial_charge_time = c.get_charge_time(edge.sourceCharger(), current.soc(), 0.7);
                labels.emplace_back(Label(label_type::CHARGE_70, connectionID, currentID,
                                          current.get_total_time() + travel_time + partial_charge_time,
                                          partial_charge_time, 0.7, &edge));
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

    return build_result(shortest_path_tree, c, sourceID, destinationID);
}

void Router::printSPT(const RouterResult &res) {

    for (int i = 0; i < res.nodes.size(); i++) {
        // print node
        if (i == 0) {
            std::cout << res.nodes[i]->id() << ": " << res.socs_out[i] << std::endl;
        } else if (i == res.nodes.size() - 1) {
            std::cout << res.nodes[i]->id() << ": " << res.socs_in[i - 1] << std::endl;
        } else {
            std::cout << res.nodes[i]->id() << ": " << res.socs_in[i - 1] << " -> " << res.socs_out[i] << " in " << res.charge_times[i] / 3600.0 << " hours (" << res.charges[i] << ")" << std::endl;
        }

        // print path
        if (i != res.nodes.size() - 1) {
            auto edge = res.arcs[i];
            std::cout << "| " << edge->get_distance() << " km @ " << edge->get_speed() << " km/h in " << edge->get_travel_time() / 3600.0 << " hours." << std::endl;
        }
    }

    std::cout << "Total journey time: " << res.total_time / 3600.0 << " hours, including " << res.charge_time / 3600.0 << " hours of charging" << std::endl;
}

RouterResult Router::build_result(const std::unordered_map<unsigned int, Label> &spt, const Car &c, unsigned sourceID, unsigned destinationID) {
    // possibly could be made more efficiently without vectors
    RouterResult res = RouterResult();
    Label current = spt.at(destinationID);
    res.total_time = current.get_total_time();


    while (current.get_nodeID() != sourceID) {
        res.socs_in.push_back(current.soc());
        res.socs_out.push_back(current.soc() + c.consumed_soc(*current.edge));
        res.charge_times.push_back(current.get_charge_time());
        res.arcs.push_back(current.edge);
        res.nodes.push_back(current.edge->destinationCharger());
        res.charges.push_back(current.type);
        current = spt.at(current.get_parentID());
    }

    res.nodes.push_back(res.arcs.back()->sourceCharger());

    for (auto t : res.charge_times) {
        res.charge_time += t;
    }

    std::reverse(res.socs_out.begin(), res.socs_out.end());
    std::reverse(res.socs_in.begin(), res.socs_in.end());
    std::reverse(res.charge_times.begin(), res.charge_times.end());
    std::reverse(res.arcs.begin(), res.arcs.end());
    std::reverse(res.nodes.begin(), res.nodes.end());
    std::reverse(res.charges.begin(), res.charges.end());

    return res;
}
