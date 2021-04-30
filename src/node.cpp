//
// Created by akowal3 on 29/04/2021.
//

#include <node.h>

unsigned Node::id() const {
    return this->ID;
}
Node::Node(unsigned ID, double SoC) : ID(ID), SoC(SoC) {}

double Node::soc() const {
    return this->SoC;
}

Node::Node() = default;