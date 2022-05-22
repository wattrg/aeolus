#ifndef __GAS_STATE_H_
#define __GAS_STATE_H_

#include <iostream>

class GasState {
public:
    double p;
    double T;
    double rho;
    double u;

    friend std::ostream& operator << (std::ostream& os, const GasState gs);
};


#endif // __GAS_STATE_H_
