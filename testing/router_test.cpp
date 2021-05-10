//
// Created by akowal3 on 10/05/2021.
//


#include <catch.hpp>
#include <custom_macros.hpp>
#include <graph_cases.hpp>
#include <iostream>
#include <router.hpp>


TEST_CASE("Simple router", "[ROUTER]") {

    Car c = Car();
    auto tc = BuildGraphCases[5];

    Router router = Router(tc.node_count, tc.graph);

    auto ret = router.route(0, 4, c);

    Router::printSPT(ret, 0, 4);

    std::cout << "Done" << std::endl;
}
