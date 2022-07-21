#include "libbc.h"

BoundaryCondition slip_wall(){
    std::vector<GhostCellEffect> ghost_cell_effects;
    ghost_cell_effects.push_back( InternalCopy() );    
    ghost_cell_effects.push_back( ReflectNormal() );

    return BoundaryCondition(ghost_cell_effects);
}

BoundaryCondition supersonic_inflow(FlowState &fs){
    std::vector<GhostCellEffect> ghost_cell_effects;
    ghost_cell_effects.push_back( FlowStateCopy(fs) );
    return BoundaryCondition(ghost_cell_effects);
}

BoundaryCondition supersonic_outflow(){
    std::vector<GhostCellEffect> ghost_cell_effects;
    ghost_cell_effects.push_back( InternalCopy() );
    return BoundaryCondition(ghost_cell_effects);
}
