#include "ghost_cell_effect.h"
#include "../../util/unused.h"

// flow state copy
void FlowStateCopy::apply(Interface &interface, Cell &valid, Cell &ghost){
    UNUSED(interface);
    UNUSED(valid);
    ghost.fs.copy(this->_fs);
}

// copy from the interior valid cell
void InternalCopy::apply(Interface &interface, Cell &valid, Cell &ghost) {
    UNUSED(interface);
    ghost.fs.copy(valid.fs);
}

// Reflect the component of the velocity normal to the interface
void ReflectNormal::apply(Interface &interface, Cell &valid, Cell &ghost) {
    UNUSED(valid);
    Interface &f = interface;
    ghost.fs.velocity.transform_to_local_frame(f.n(), f.t1(), f.t2());
    ghost.fs.velocity.x = -ghost.fs.velocity.x;
    ghost.fs.velocity.transform_to_global_frame(f.n(), f.t1(), f.t2());
}
