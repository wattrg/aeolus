#ifndef __FLUID_BLOCK_H_
#define __FLUID_BLOCK_H_

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include "../util/array.h"
#include "boundary_conditions/boundary_condition.h"
#include "cell.h"
#include "interface.h"
#include "vertex.h"
#include "../gas/flow_state.h"
#include "../gas/gas_model.h"
#include "omp.h"

class FluidBlockIO;


class FluidBlock {
public:
    ~FluidBlock();
    FluidBlock(Grid::Grid & grid, 
               unsigned int id, 
               std::map<std::string, BoundaryCondition> &);

    /// String representation of the FluidBlock
    std::string to_string();
    friend std::ostream& operator << (std::ostream& os, const FluidBlock fluid_block);

    // set the function to use to fill the fluid block with data
    void fill(std::function<FlowState(double, double, double)> &func);
    void fill(const FlowState &);

    void compute_fluxes();
    void compute_time_derivatives();
    void apply_time_derivative();
    double compute_block_dt(double cfl);
    void reconstruct();

    // getter methods
    std::vector<Cell> & cells();
    std::vector<Vertex> & vertices();
    std::vector<Interface> & interfaces();
    std::vector<BoundaryCondition> bcs() {return _bcs;}
    unsigned int id() const {return this->_id;}

private:
    // Collection of cells
    std::vector<Cell> _cells;

    // ghost cells
    std::vector<Cell> _ghost_cells;

    // Collection of interfaces
    std::vector<Interface> _interfaces;

    // Collection of vertices
    std::vector<Vertex> _vertices;

    // boundary conditions for this fluid block
    std::vector<BoundaryCondition> _bcs;

    // dt for the whole grid
    double _dt;

    // check if the fluid block already has
    // a particular interface
    bool _has_interface(Interface interface);

    // check if the fluid block already has
    // a particular vertex
    bool _has_vertex(Vertex vertex);

    void _print_interfaces();

    FluidBlockIO * _fb_io;

    // id of the fluid block
    unsigned int _id;
};


#endif // __FLUIDBLOCK_H_
