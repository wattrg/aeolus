#include "solvers.h"

Solver::Solver(Simulation & config) : _config(config) {}

void Solver::solve(){
    double dt;
    for (unsigned int step=0; step < this->_max_step; step++){
        dt = this->_step(); 
        this->_config.add_time_increment(dt);
    }
}
