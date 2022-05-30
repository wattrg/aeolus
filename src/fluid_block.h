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

    // Add an interface to the fluid block if it doesn't already exist
    Interface & add_interface(std::vector<Vertex> vertices);


    friend std::ostream& operator << (std::ostream& os, const FluidBlock fluid_block);

private:
    // Collection of cells
    std::vector<Cell> _cells;

    // Collection of interfaces
    std::vector<Interface> _interfaces;

    // Collection of vertices
    std::vector<Vertex> _vertices;

    // check if the fluid block already has
    // a particular interface
    bool _has_interface(Interface interface);

    // check if the fluid block already has
    // a particular vertex
    bool _has_vertex(Vertex vertex);

    void _print_interfaces();
};


#endif // __FLUIDBLOCK_H_
