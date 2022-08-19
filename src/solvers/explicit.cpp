#include "explicit.h"

double ExplicitSolver::_step(){
    // apply pre-reconstruction boundary conditions
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        for (BoundaryCondition * bc : fb->bcs()){
            bc->apply_pre_reconstruction();
        }
    }
    double dt; 
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        fb->reconstruct();
        fb->compute_fluxes();
        fb->compute_time_derivatives();
        dt = fb->compute_block_dt();
        fb->apply_time_derivative();
    }
    return dt;
}
