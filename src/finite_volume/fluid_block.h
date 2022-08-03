#ifndef __FLUID_BLOCK_H_
#define __FLUID_BLOCK_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include "cell.h"
#include "interface.h"
#include "vertex.h"
#include "flow_state.h"
#include "../gas/gas_model.h"
#include "config.h"
#include "io/grid_io.h"
#include "io/fluid_block_io.h"

class GridIO;
class FluidBlockIO;

namespace ElementShape {
    enum ElementShape : unsigned short { Quad, Line, };
}

class FluidBlock {
public:
    FluidBlock(const char * file_name);

    /// String representation of the FluidBlock
    std::string to_string();
    friend std::ostream& operator << (std::ostream& os, const FluidBlock fluid_block);

    // set the function to use to fill the fluid block with data
    void fill_function(std::function<FlowState(double, double, double)> &func);

    GlobalConfig fb_config;

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

    friend class GridIO;
    friend class Su2GridInput;

    GridIO * _grid_io;
    FluidBlockIO * _fb_io;
};


#endif // __FLUIDBLOCK_H_
