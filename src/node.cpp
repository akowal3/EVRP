//
// Created by akowal3 on 29/04/2021.
//

#include <node.h>

unsigned Node::id() const {
    return this->ID;
}
Node::Node(unsigned ID, double SoC) : ID(ID),
                                      charge_level(SoC) {}

double Node::soc() const {
    return this->charge_level;
}

Node::Node() = default;