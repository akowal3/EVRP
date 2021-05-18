//
// Created by akowal3 on 07/05/2021.
//

#include <label.hpp>
#include <router.hpp>

unsigned Label::labelCounter = 0;

Label::Label(label_type type, unsigned int nodeID, unsigned int parentID, Time total_time, Time charge_time, double remaining_soc, const Edge *edge) :
    nodeID(nodeID), parentID(parentID), total_time(total_time), charge_time(charge_time),
    remaining_soc(remaining_soc), edge(edge), labelID(labelCounter++), type(type) {}

Label::Label(unsigned int nodeID, double SoC) :
    nodeID(nodeID), parentID(nodeID), total_time(0), charge_time(0),
    remaining_soc(SoC), edge(nullptr), labelID(labelCounter++), type(label_type::INITIAL_DUMMY) {}

bool Label::operator<(const Label &other) const {
    return total_time < other.total_time;
}

bool Label::operator>(const Label &other) const {
    return total_time > other.total_time;
}

// this Label dominates other label when travel time is lower and soc_left is higher
// Potentially these conditions should be relaxed.
bool Label::dominates(const Label &other) const {
    return time_cmp(total_time, OP::SMALLER, other.total_time) &&
           soc_cmp(remaining_soc, OP::GREATER_EQUAL, other.remaining_soc);
}

unsigned Label::get_nodeID() const { return nodeID; }
unsigned Label::get_parentID() const { return parentID; }
unsigned Label::get_labelID() const { return labelID; }
double Label::soc() const { return remaining_soc; }
Time Label::get_total_time() const { return total_time; }
Time Label::get_charge_time() const { return charge_time; }
