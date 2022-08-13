#ifndef __CONSERVED_QUANTITIES_H_
#define __CONSERVED_QUANTITIES_H_

#include "../util/vector.h"
#include <vector>

class ConservedQuantity{
public:
    ConservedQuantity();
    ConservedQuantity(unsigned int number_dimensions);
    std::vector<double> conserved_quantities;
    double & rho() {return conserved_quantities[_rho_idx];}
    double * momentum() {return  &conserved_quantities[_momentum_idx];}
    double & energy() { return conserved_quantities[_energy_idx]; }
    unsigned int n_conserved_quantities() {return _n_conserved_quantities; }
    const unsigned int dimensions() const;

private:
    int _rho_idx = 0;
    int _momentum_idx = 1;
    int _energy_idx;
    unsigned int _n_conserved_quantities;
    unsigned int _number_dimensions;
};

#endif // __CONSERVED_QUANTITIES_H_
