#ifndef __GAS_STATE_H_
#define __GAS_STATE_H_

#include <iostream>
#include "gas_model.h"

class GasModel;

class GasState {
public:
    GasState();
    GasState(GasModel * gm);
    double p;
    double T;
    double rho;
    double u;
    
    // speed of sound
    double a;

    std::string to_string() const;
    friend std::ostream& operator << (std::ostream& os, const GasState gs);

    void update_from_pT();
    void update_from_prho();
    void update_from_rhoT();
    void update_from_rhou();

    // copy the values from some other gas state
    void copy(const GasState & other);

    // compute the internal energy
    double internal_energy();
};


#endif // __GAS_STATE_H_
