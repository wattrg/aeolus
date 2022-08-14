#ifndef __CONSERVED_QUANTITIES_H_
#define __CONSERVED_QUANTITIES_H_

#include "../util/vector.h"
#include <vector>

class ConservedQuantity{
public:
    ConservedQuantity();
    ConservedQuantity(unsigned int number_dimensions);
    unsigned int rho() {return _rho_idx;}
    unsigned int momentum() {return _momentum_idx;}
    unsigned int energy() { return _energy_idx; }
    unsigned int n_conserved() {return _n_conserved_quantities; }
    const unsigned int dimensions() const;
    double &operator [] (unsigned int index) { return _conserved_quantities[index]; }

private:
    std::vector<double> _conserved_quantities;
    unsigned int _rho_idx = 0;
    unsigned int _momentum_idx = 1;
    unsigned int _energy_idx;
    unsigned int _n_conserved_quantities;
    unsigned int _number_dimensions;
};

#endif // __CONSERVED_QUANTITIES_H_
