#ifndef __GAS_STATE_H_
#define __GAS_STATE_H_

#include <iostream>

class GasState {
public:
    GasState();
    double p;
    double T;
    double rho;
    double u;

    std::string to_string() const;
    friend std::ostream& operator << (std::ostream& os, const GasState gs);
};


#endif // __GAS_STATE_H_
