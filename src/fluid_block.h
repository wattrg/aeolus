#ifndef __FLUID_BLOCK_H_
#define __FLUID_BLOCK_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "cell.h"
#include "interface.h"
#include "vertex.h"
#include "flow_state.h"
#include "gas_model.h"

class FluidBlock {
public:
    FluidBlock(const char * file_name);
private:
    // Collection of cells
    std::vector<Cell> _cells;

    // Collection of interfaces
    std::vector<Interface> _interfaces;

    // Collection of vertices
    std::vector<Vertex> _vertices;
};


#endif // __FLUIDBLOCK_H_
