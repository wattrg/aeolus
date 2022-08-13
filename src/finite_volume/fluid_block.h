#ifndef __FLUID_BLOCK_H_
#define __FLUID_BLOCK_H_

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include "cell.h"
#include "interface.h"
#include "vertex.h"
#include "../gas/flow_state.h"
#include "../gas/gas_model.h"
#include "config.h"
#include "io/grid_io.h"
#include "io/fluid_block_io.h"
#include "boundary_conditions/boundary_condition.h"

class GridIO;
class FluidBlockIO;

namespace ElementShape {
    enum ElementShape : unsigned short { Quad, Line, };
}

class FluidBlock {
public:
    ~FluidBlock();
    FluidBlock(const char * file_name, Simulation & config, unsigned int id, 
            std::map<std::string, BoundaryCondition> &);

    /// String representation of the FluidBlock
    std::string to_string();
    friend std::ostream& operator << (std::ostream& os, const FluidBlock fluid_block);

    // set the function to use to fill the fluid block with data
    void fill_function(std::function<FlowState(double, double, double)> &func);

    Simulation & fb_config;

    void compute_fluxes();
    void compute_time_derivatives();
    void apply_time_derivative();
    void compute_block_dt();

    // getter methods
    const std::vector<Cell *> & cells() const;
    const std::vector<Vertex *> & vertices() const;
    std::vector<Interface *> & interfaces();
    std::vector<BoundaryCondition *> bcs() {return _bcs;}
    const unsigned int id() const {return this->_id;}

    // setter method
    void set_grid(std::vector<Vertex *> vertices, 
                  std::vector<Interface *> interfaces, 
                  std::vector<Cell *> cells, 
                  std::vector<Cell *> ghost_cells,
                  std::vector<BoundaryCondition *> bcs);

private:
    // Collection of cells
    std::vector<Cell *> _cells;

    // ghost cells
    std::vector<Cell *> _ghost_cells;

    // Collection of interfaces
    std::vector<Interface *> _interfaces;

    // Collection of vertices
    std::vector<Vertex *> _vertices;

    // boundary conditions for this fluid block
    std::vector<BoundaryCondition *> _bcs;

    // dt for the whole grid
    double _dt;

    // check if the fluid block already has
    // a particular interface
    bool _has_interface(Interface interface);

    // check if the fluid block already has
    // a particular vertex
    bool _has_vertex(Vertex vertex);

    void _print_interfaces();

    GridIO * _grid_io;
    FluidBlockIO * _fb_io;

    // id of the fluid block
    unsigned int _id;
};


#endif // __FLUIDBLOCK_H_
