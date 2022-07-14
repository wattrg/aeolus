#ifndef __CONSERVED_QUANTITIES_H_
#define __CONSERVED_QUANTITIES_H_

#include "vector.h"
#include "config.h"
#include <vector>

class ConservedQuantity{
public:
    ConservedQuantity();
    ConservedQuantity(GlobalConfig & config);
    std::vector<double> conserved_quantities;
    double & rho() {return conserved_quantities[_rho_idx];}
    double & momentum() {return conserved_quantities[_momentum_idx];}
    double & energy() { return conserved_quantities[_energy_idx]; }
    int n_conserved_quantities() {return _n_conserved_quantities; }

private:
    int _rho_idx = 0;
    int _momentum_idx = 1;
    int _energy_idx;
    int _n_conserved_quantities;
};

#endif // __CONSERVED_QUANTITIES_H_
