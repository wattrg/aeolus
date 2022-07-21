// define some commonly used boundary conditions to expose to python

#ifndef __LIBBC_
#define __LIBBC_

#include <vector>
#include "../boundary_conditions/boundary_condition.h"
#include "../boundary_conditions/ghost_cell_effect.h"
#include "../flow_state.h"

BoundaryCondition slip_wall();
BoundaryCondition supersonic_outflow();
BoundaryCondition supersonic_inflow(FlowState &fs);

#endif


