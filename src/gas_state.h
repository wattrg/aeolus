#ifndef __GAS_STATE_H_
#define __GAS_STATE_H_

#include <iostream>
#include "gas_model.h"

class GasModel;

class GasState {
public:
    GasState();
    GasState(GasModel gm);
    ~GasState();
    double p;
    double T;
    double rho;
    double u;

    std::string to_string() const;
    friend std::ostream& operator << (std::ostream& os, const GasState gs);

    void update_from_pT();
    void update_from_prho();
    void update_from_rhoT();

private:
    GasModel * _gm;
};


#endif // __GAS_STATE_H_
