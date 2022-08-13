#include "explicit.h"

void ExplicitSolver::_step(){
    // apply pre-reconstruction boundary conditions
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        for (BoundaryCondition * bc : fb->bcs()){
            bc->apply_pre_reconstruction();
        }
    }
    
    // loop over the cells to compute the time derivatives in each cell
    // also, compute the dt for each cell, keeping track of the minimum one
    double dt = 1000000.0;
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        for (Cell * cell : fb->cells()){
            dt = std::min(cell->compute_local_timestep(), dt);
            cell->compute_time_derivative();
        }
    }
    
    for (FluidBlock * fb : this->_config.fluid_blocks()){
        for (Cell * cell : fb->cells()){
            ConservedQuantity cq = cell->conserved_quantities;
            for (unsigned int i=0; i < cq.n_conserved_quantities(); i++){
                cq.conserved_quantities[i] += cell->residual.conserved_quantities[i] * this->_cfl * dt;
            }
        }
    }
}
