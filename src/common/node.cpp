//
// Created by akowal3 on 29/04/2021.
//

#include <node.hpp>

Node::Node(unsigned ID, double SoC) :
    ID(ID), charge_level(SoC) {}

Node::Node(unsigned ID) :
    ID(ID), charge_level(100.0) {}

unsigned Node::id() const {
    return this->ID;
}

double Node::soc() const {
    return this->charge_level;
}

Node::Node() = default;