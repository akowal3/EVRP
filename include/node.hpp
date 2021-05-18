//
// Created by akowal3 on 29/04/2021.
//

#ifndef EVRP_NODE_HPP
#define EVRP_NODE_HPP

#include <car.hpp>
#include <utils.hpp>

class Node {
private:
    unsigned ID;
    double charge_level;// charge car only up to charge_level % of cars battery capacity. For Graph traversal
    charger_type type;

public:
    Node(unsigned ID, double SoC, charger_type type);
    Node(unsigned ID, double SoC);
    Node(unsigned int ID);
    unsigned id() const;
    double soc() const;
    charger_type best_compatible_type(const Car &c) const;
};


#endif//EVRP_NODE_HPP
