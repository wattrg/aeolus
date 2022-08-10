#include "conserved_quantities.h"

ConservedQuantity::ConservedQuantity() : _energy_idx(-1) {}

ConservedQuantity::ConservedQuantity(unsigned int number_dimensions): 
    _number_dimensions(number_dimensions)
{
    this->_energy_idx = this->_momentum_idx + this->_number_dimensions;
    this->_n_conserved_quantities = _energy_idx + 1;
    conserved_quantities = std::vector<double>(this->_n_conserved_quantities);
}

const unsigned int ConservedQuantity::dimensions() const {
    return this->_number_dimensions;
}
