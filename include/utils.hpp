//
// Created by akowal3 on 13/05/2021.
//

#ifndef EVRP_UTILS_HPP
#define EVRP_UTILS_HPP

#include <ostream>

typedef unsigned Time;// (s)

// Specifies the possible choices made by the algorithm at each charger.
enum class label_type {
    INITIAL_DUMMY,
    NO_CHARGE,
    CHARGE_MAXIMUM,
    CHARGE_MINIMUM,
    CHARGE_70,
    CHARGE_80,
};

enum class OP {
    GREATER,
    GREATER_EQUAL,
    EQUAL,
    SMALLER,
    SMALLER_EQUAL,
    NOT_EQUAL,
};

enum class charger_type {
    FAST_175KW,
    SLOW_50KW,
    NO_CHARGER,
};

bool soc_cmp(double left, OP operand, double right);
bool time_cmp(Time left, OP operand, Time right);
std::ostream &operator<<(std::ostream &os, const label_type &type);
std::ostream &operator<<(std::ostream &os, const charger_type &type);

#endif//EVRP_UTILS_HPP
