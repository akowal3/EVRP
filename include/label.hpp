//
// Created by akowal3 on 07/05/2021.
//

#ifndef EVRP_LABEL_HPP
#define EVRP_LABEL_HPP

#include <edge.h>

// Label is one particular way of getting from parentID to nodeID.
// It can have different charges, travel speeds etc.
class Label {
private:
    unsigned nodeID;
    unsigned parentID;
    unsigned labelID;// unique among labels

    double remaining_soc;  // soc_left upon arrival to head
    Time total_time;       // total time spent traveling for the whole search path
    double consumed_energy;// energy consumed when traveling from tail to head
    Time charge_time;      // time spent charging at the tail node

    static unsigned labelCounter;

public:
    Label(unsigned currentID, unsigned parentID, Time total_time, Time charge_time, double remaining_soc, double consumed_energy = 0.0);
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
};


#endif//EVRP_LABEL_HPP
