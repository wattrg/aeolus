#ifndef __SU2_H_
#define __SU2_H_

#include "grid_io.h"
#include "../fluid_block.h"


class Su2GridInput : public GridInput {
    void read_grid(const char *, FluidBlock &, std::map<std::string, BoundaryCondition *> &);
};

#endif
