//
// Created by akowal3 on 29/04/2021.
//

#include <node.hpp>

Node::Node(unsigned int ID, double SoC, charger_type type) :
    ID(ID), charge_level(SoC), type(type) {}

Node::Node(unsigned ID, double SoC) :
    ID(ID), charge_level(SoC), type(charger_type::FAST_175KW) {}

Node::Node(unsigned ID) :
    ID(ID), charge_level(100.0), type(charger_type::FAST_175KW) {}

unsigned Node::id() const {
    return this->ID;
}

double Node::soc() const {
    return this->charge_level;
}

// can be extended to chargers having more than one type (as vector) and cars not supporting all kinds of chargers.
charger_type Node::best_compatible_type(const Car &c) const {
    // TODO: this assumes that charger has only one type and every car is compatible with every charger type
    return this->type;
}
std::vector<charger_type> Node::supported_types() const {
    return { this->type };
}

Node &Node::operator=(const Node &other) {
    this->ID = other.ID;
    this->charge_level = other.charge_level;
    this->type = other.type;

    return *this;
}

bool operator==(const Node &left, const Node &right) {
    return left.type == right.type && left.charge_level == right.charge_level && left.ID == right.ID;
}
