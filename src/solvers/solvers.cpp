#include "solvers.h"

Solver::Solver(Simulation & config) : _config(config) {}

void Solver::solve(){
    double dt;
    for (unsigned int step=0; step < this->_max_step; step++){
        try{
            dt = this->_step(); 
        }
        catch (std::runtime_error & e) {
            this->_config.write_fluid_blocks();
            throw std::runtime_error(e);
        }
        this->_config.add_time_increment(dt);
        this->_config.write_fluid_blocks();
    }
}
