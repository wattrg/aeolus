#include "explicit.h"

void ExplicitSolver::_step(){
    // apply pre-reconstruction boundary conditions
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        for (BoundaryCondition * bc : fb->bcs()){
            bc->apply_pre_reconstruction();
        }
    }
    
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        fb->compute_fluxes();
        fb->compute_time_derivatives();
        fb->compute_block_dt();
        fb->apply_time_derivative();
    }
}
