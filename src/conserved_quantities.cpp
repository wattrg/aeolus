#include "conserved_quantities.h"

ConservedQuantity::ConservedQuantity() : _energy_idx(-1) {}

ConservedQuantity::ConservedQuantity(GlobalConfig & config){
    this->_energy_idx = this->_momentum_idx + config.dimensions;
}
