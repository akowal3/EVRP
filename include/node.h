//
// Created by akowal3 on 29/04/2021.
//

#ifndef EVRP_NODE_H
#define EVRP_NODE_H


class Node {
private:
    unsigned ID;
    double SoC;// charge car only up to soc_level % of cars battery capacity
public:
    unsigned id() const;
    double soc() const;
    Node(unsigned ID, double SoC);
    Node();
};


#endif//EVRP_NODE_H
