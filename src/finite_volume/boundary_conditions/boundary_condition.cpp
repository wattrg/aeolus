#include "boundary_condition.h"

void BoundaryCondition::apply_pre_reconstruction(){
    for (Cell * ghost_cell : this->_ghost_cells){
        for (GhostCellEffect & action : this->_pre_recon_actions){
            action.apply(*ghost_cell);
        }
    }
}

