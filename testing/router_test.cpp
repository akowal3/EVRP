//
// Created by akowal3 on 10/05/2021.
//


#include <catch.hpp>
#include <graph_cases.hpp>
#include <router.hpp>
#include <sstream>
#include <utils.hpp>

TEST_CASE("Simple router verifier", "[ROUTER]") {
    for (auto &tc : RKCases) {
        GIVEN(tc.description) {
            Router router = Router(tc.node_count, tc.graph);

            unsigned sourceID = tc.path.source_original();
            unsigned destinationID = tc.path.target();
            double initial_soc = tc.path.charge_levels.front();// same as for the graph

            Car c = Car(initial_soc);

            RouterResult ret = router.route(sourceID, destinationID, c);
            //            auto total_time = ret.at(destinationID).get_total_time();
            //

            THEN("Calculated path goes through the correct nodes") {
                REQUIRE(ret.nodes.size() == tc.path.nodes.size());
                for (int i = 0; i < ret.nodes.size(); i++) {
                    std::ostringstream msg;

                    if (i == 0) {
                        msg << ret.nodes[i]->id() << ": " << ret.socs_out[i];
                    } else if (i == ret.nodes.size() - 1) {
                        msg << ret.nodes[i]->id() << ": " << ret.socs_in[i - 1] << std::endl;
                    } else {
                        msg << ret.nodes[i]->id() << ": " << ret.socs_in[i - 1] << " -> " << ret.socs_out[i] << " in " << ret.charge_times[i] / 3600.0 << " hours (" << ret.charges[i] << ")" << std::endl;
                    }

                    WHEN(msg.str()) {
                        REQUIRE(ret.nodes[i]->id() == tc.path.nodes[i]);
                    }
                }
            }

            THEN("Calculated path takes edges with expected speeds") {
                REQUIRE(ret.arcs.size() == tc.path.speeds.size());
                for (int i = 0; i < ret.arcs.size(); i++) {
                    auto edge = ret.arcs[i];
                    auto tailID = edge->tailID();
                    auto headID = edge->headID();
                    WHEN(tailID << " -> " << headID << " | " << edge->get_distance() << " km @ " << edge->get_speed() << " km/h in " << edge->get_travel_time() / 3600.0 << " hours") {
                        REQUIRE(edge->get_speed() == tc.path.speeds[i]);
                        REQUIRE(tailID == tc.path.nodes[i]);
                        if (i == ret.arcs.size() - 1) {
                            // for last edge verify the head to match last destinationCharger
                            REQUIRE(headID == tc.path.nodes.back());
                        }
                    }
                }
            }
            AND_THEN("Route takes " << ret.total_time / 3600.0 << " hours (" << ret.total_time << " seconds). Including " << ret.charge_time / 3600.0 << " hours of charging"){};
        }
    }
}
