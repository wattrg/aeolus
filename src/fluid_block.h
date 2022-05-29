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

    // add a vertex at position `pos` to the fluid block. In the vertex
    // already exists, don't repeat it
    Vertex & add_vertex(Vector2 pos);

    // add an interface from vertex `start` to `end` to the fluid block
    // If the interface already exists, don't repeat it
    Interface & add_interface(Vertex & start, Vertex & end);

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

};


#endif // __FLUIDBLOCK_H_
