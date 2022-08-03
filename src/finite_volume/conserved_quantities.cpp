#include "conserved_quantities.h"

ConservedQuantity::ConservedQuantity() : _energy_idx(-1) {}

ConservedQuantity::ConservedQuantity(GlobalConfig & config): cq_config(&config){
    this->_energy_idx = this->_momentum_idx + config.dimensions;
    this->_n_conserved_quantities = _energy_idx + 1;
    conserved_quantities = std::vector<double>(this->_n_conserved_quantities);
}
