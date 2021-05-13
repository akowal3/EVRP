//
// Created by akowal3 on 29/04/2021.
//

#ifndef EVRP_NODE_HPP
#define EVRP_NODE_HPP

class Node {
private:
    unsigned ID;
    double charge_level;// charge car only up to charge_level % of cars battery capacity
public:
    unsigned id() const;
    double soc() const;
    Node(unsigned ID, double SoC);
    Node();
    Node(unsigned int ID);
};


#endif//EVRP_NODE_HPP
