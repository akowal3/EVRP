//
// Created by akowal3 on 13/05/2021.
//

#include <utils.hpp>

#include <cmath>

// assumes the inputs correspond to SoC values and the SoC value is only correct to 1 decimal place ie. XX.X
bool soc_cmp(double left, OP operand, double right) {
    constexpr static double epsilon = 0.001;
    bool equal = std::abs(left - right) < epsilon;
    bool greater = left - right >= epsilon;
    switch (operand) {
        case OP::GREATER:
            return greater;
        case OP::GREATER_EQUAL:
            return greater || equal;
        case OP::EQUAL:
            return equal;
        case OP::SMALLER:
            return !greater && !equal;
        case OP::SMALLER_EQUAL:
            return !greater;
        case OP::NOT_EQUAL:
            return !equal;
    }
}

bool time_cmp(Time left, OP operand, Time right) {
    constexpr static long epsilon = 60;// treat 1 minute difference as no difference
    bool equal = std::abs((long) left - (long) right) < epsilon;
    bool greater = (long) left - (long) right >= epsilon;
    switch (operand) {
        case OP::GREATER:
            return greater;
        case OP::GREATER_EQUAL:
            return greater || equal;
        case OP::EQUAL:
            return equal;
        case OP::SMALLER:
            return !greater && !equal;
        case OP::SMALLER_EQUAL:
            return !greater;
        case OP::NOT_EQUAL:
            return !equal;
    }
}

std::ostream &operator<<(std::ostream &os, const label_type &type) {
    switch (type) {
        case label_type::INITIAL_DUMMY:
            os << "INITIAL_DUMMY";
            break;
        case label_type::NO_CHARGE:
            os << "NO_CHARGE";
            break;
        case label_type::CHARGE_MAXIMUM:
            os << "CHARGE_MAXIMUM";
            break;
        case label_type::CHARGE_MINIMUM:
            os << "CHARGE_MINIMUM";
            break;
        case label_type::CHARGE_70:
            os << "CHARGE_70";
            break;
    }
    return os;
}