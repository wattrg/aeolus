#include "solvers.h"

Solver::Solver(GlobalConfig & config) : _config(config) {}

void Solver::solve(){
    for (unsigned int step=0; step < this->_config.max_step(); step++){
        this->_step(); 
    }
}
