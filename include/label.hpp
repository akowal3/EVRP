//
// Created by akowal3 on 07/05/2021.
//

#ifndef EVRP_LABEL_HPP
#define EVRP_LABEL_HPP

#include <edge.h>

class Label {
private:
    unsigned tailID;
    unsigned headID;
    unsigned labelID;// unique among labels

    double remaining_soc;  // soc_left upon arrival to head
    Time total_time;       // total time spent traveling for the whole search path
    double consumed_energy;// energy consumed when traveling from head to tailID
    Time charge_time;      // time spent charging at the tailID node

    static unsigned labelCounter;

public:
    Label(unsigned tail, unsigned head, Time total_time, Time charge_time, double remaining_soc, double consumed_energy = 0.0);
    Label(unsigned nodeID, double SoC);
    bool dominates(const Label &other) const;
    bool operator<(const Label &other) const;
    bool operator>(const Label &other) const;
    unsigned get_headID() const;
    unsigned get_tailID() const;
    unsigned get_labelID() const;
    Time get_total_time() const;
    double soc() const;
};


#endif//EVRP_LABEL_HPP
