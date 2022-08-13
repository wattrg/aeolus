#include "explicit.h"

void ExplicitSolver::solve(){
    // apply pre-reconstruction boundary conditions
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        for (BoundaryCondition * bc : fb->bcs()){
            bc->apply_pre_reconstruction();
        }
    }
    
    // loop over the cells to compute the time derivatives in each cell
    // also, compute the dt for each cell, keeping track of the minimum one
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        for (Cell * cell : fb->cells()){
            // compute time step size
            cell->compute_time_derivative();
        }
    }


}
