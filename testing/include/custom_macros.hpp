//
// Created by akowal3 on 03/05/2021.
//

#ifndef EVRP_CUSTOM_MACROS_HPP
#define EVRP_CUSTOM_MACROS_HPP

#include <catch.hpp>

#define REQUIRE_FAILMSG(cond, msg) \
    {                              \
        INFO(msg);                 \
        REQUIRE(cond);             \
    }

#define CHECK_FAILMSG(cond, msg) \
    {                            \
        INFO(msg);               \
        CHECK(cond);             \
    }


#endif//EVRP_CUSTOM_MACROS_HPP
