#include "explicit.h"

double ExplicitSolver::_step(){
    // apply pre-reconstruction boundary conditions
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        fb->apply_bcs();
    }
    double dt = -1; 
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        fb->reconstruct();
        fb->compute_fluxes();
        fb->compute_residuals();
        dt = fb->compute_block_dt(this->_cfl);
        fb->apply_residuals(dt);
    }
    return dt;
}
