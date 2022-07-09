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

namespace ElementShape {
    enum ElementShape { Quad, Line, };
}

class FluidBlock {
public:
    FluidBlock(const char * file_name);

    void fill_initial_condition();

    friend std::ostream& operator << (std::ostream& os, const FluidBlock fluid_block);

private:
    // Collection of cells
    std::vector<Cell *> _cells;

    // Collection of interfaces
    std::vector<Interface *> _interfaces;

    // Collection of vertices
    std::vector<Vertex *> _vertices;

    // check if the fluid block already has
    // a particular interface
    bool _has_interface(Interface interface);

    // check if the fluid block already has
    // a particular vertex
    bool _has_vertex(Vertex vertex);

    void _print_interfaces();


    // Add an interface to the fluid block if it doesn't already exist
    Interface * _add_interface(std::vector<Vertex *> vertices);

    // Find the interface with specified vertices.
    // Return null pointer if the interface doesn't exist
    Interface * _find_interface(std::vector<Vertex *> vertices);
};


#endif // __FLUIDBLOCK_H_
