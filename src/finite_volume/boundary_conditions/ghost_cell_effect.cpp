#include "ghost_cell_effect.h"
#include "../../util/unused.h"

// flow state copy
void FlowStateCopy::apply(Interface &, FlowState &, FlowState &ghost){
    ghost.copy(this->_fs);
}

// copy from the interior valid cell
void InternalCopy::apply(Interface &, FlowState &valid, FlowState &ghost) {
    ghost.copy(valid);
}

// Reflect the component of the velocity normal to the interface
void ReflectNormal::apply(Interface &interface, FlowState &, FlowState &ghost) {
    Interface &f = interface;
    ghost.velocity.transform_to_local_frame(f.n(), f.t1(), f.t2());
    ghost.velocity.x = -ghost.velocity.x;
    ghost.velocity.transform_to_global_frame(f.n(), f.t1(), f.t2());
}
