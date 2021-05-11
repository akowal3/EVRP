//
// Created by akowal3 on 07/05/2021.
//

#ifndef EVRP_LABEL_HPP
#define EVRP_LABEL_HPP

#include <edge.h>
#include <ostream>
// Label is one particular way of getting from parentID to nodeID.
// It can have different charges, travel speeds etc.

enum class label_type {
    INITIAL_DUMMY,
    NO_CHARGE,
    CHARGE_MAXIMUM,
    CHARGE_MINIMUM,
    CHARGE_70,
};


class Label {
    friend class Router;

public:
    Label(label_type type, unsigned int nodeID, unsigned int parentID, Time total_time, Time charge_time, double remaining_soc, const Edge *edge);
    Label(unsigned nodeID, double SoC);
    bool dominates(const Label &other) const;
    bool operator<(const Label &other) const;
    bool operator>(const Label &other) const;
    unsigned get_nodeID() const;
    unsigned get_parentID() const;
    unsigned get_labelID() const;
    Time get_total_time() const;
    double soc() const;
    Time get_charge_time() const;

private:
    unsigned nodeID;
    unsigned parentID;
    unsigned labelID;// unique among labels
    label_type type;

    double remaining_soc;// soc_left upon arrival to head
    Time total_time;     // total time spent traveling for the whole search path
    Time charge_time;    // time spent charging at the tail node
    const Edge *edge;    // edge taken to traverse the route

    static unsigned labelCounter;
};


#endif//EVRP_LABEL_HPP
