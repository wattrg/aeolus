#ifndef __BOUNDARY_CONDITION_
#define __BOUNDARY_CONDITION_

#include "../cell.h"
#include "../interface.h"
#include "ghost_cell_effect.h"
#include <vector>

class BoundaryCondition{

public:
    BoundaryCondition(std::vector<GhostCellEffect> pre_recon);

    // apply the pre-reconstruction actions
    void apply_pre_reconstruction();

private:
    // keep track of the ghost cells and interfaces
    // for this particular boundary condition
    std::vector<Cell *> _ghost_cells;
    std::vector<Interface*> _interfaces;

    // actions to perform
    std::vector<GhostCellEffect> _pre_recon_actions;
};

#endif
