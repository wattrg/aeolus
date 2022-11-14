#ifndef __CONSERVED_QUANTITIES_H_
#define __CONSERVED_QUANTITIES_H_

#include "../util/vector3.h"
#include <vector>

class ConservedQuantity{
public:
    ConservedQuantity(){};

    double mass = 0.0;
    Vector3 momentum;
    double energy = 0.0;
};

#endif // __CONSERVED_QUANTITIES_H_
