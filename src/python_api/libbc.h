// define some commonly used boundary conditions to expose to python

#ifndef __LIBBC_
#define __LIBBC_

#include <vector>
#include "../finite_volume/boundary_conditions/boundary_condition.h"
#include "../finite_volume/boundary_conditions/ghost_cell_effect.h"
#include "../finite_volume/flow_state.h"

BoundaryCondition slip_wall();
BoundaryCondition supersonic_outflow();
BoundaryCondition supersonic_inflow(FlowState &fs);

#endif


