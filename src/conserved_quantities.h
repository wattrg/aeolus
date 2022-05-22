#ifndef __CONSERVED_QUANTITIES_H_
#define __CONSERVED_QUANTITIES_H_

#include "vector.h"

class ConservedQuantity{
public:
    double rho;
    Vector2 momentum;
    double energy;
};

#endif // __CONSERVED_QUANTITIES_H_
