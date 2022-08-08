#ifndef __CONSERVED_QUANTITIES_H_
#define __CONSERVED_QUANTITIES_H_

#include "../util/vector.h"
#include "config.h"
#include <vector>

class ConservedQuantity{
public:
    ConservedQuantity();
    ConservedQuantity(GlobalConfig & config);
    GlobalConfig * cq_config;
    std::vector<double> conserved_quantities;
    double & rho() {return conserved_quantities[_rho_idx];}
    double * momentum() {return  &conserved_quantities[_momentum_idx];}
    double & energy() { return conserved_quantities[_energy_idx]; }
    const int n_conserved_quantities() const {return _n_conserved_quantities; }
    double & operator [] (int i);

private:
    int _rho_idx = 0;
    int _momentum_idx = 1;
    int _energy_idx;
    int _n_conserved_quantities;
};

#endif // __CONSERVED_QUANTITIES_H_
