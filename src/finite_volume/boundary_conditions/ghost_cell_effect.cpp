#include "ghost_cell_effect.h"

// Fixed PT
FlowStateCopy::FlowStateCopy(FlowState & fs) : _fs(fs) {}

void FlowStateCopy::apply(Cell & ghost_cell){
    ghost_cell.fs.copy(this->_fs);
}

// copy from the interior valid cell
void InternalCopy::apply(Cell & ghost_cell) {
    // the ghost cell only has one interface -- the one with the 
    // valid cell. So we find the cell on the other side
    // of that interface
    CellFace & face = ghost_cell._interfaces[0];
    Cell & valid_cell = face.interface->get_valid_cell(); 
    ghost_cell.fs.copy(valid_cell.fs);
}

// copy from the interior valid cell
void ReflectNormal::apply(Cell & ghost_cell) {
    Interface & f = *ghost_cell._interfaces[0].interface;

    ghost_cell.fs.velocity.transform_to_local_frame(f.n(), f.t1(), f.t2());
    ghost_cell.fs.velocity.x = -ghost_cell.fs.velocity.x;
    ghost_cell.fs.velocity.transform_to_global_frame(f.n(), f.t1(), f.t2());
}
