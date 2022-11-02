#ifndef __FLUID_BLOCK_H_
#define __FLUID_BLOCK_H_

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>
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

    // fluid dynamic methods
    void compute_fluxes();
    void compute_time_derivatives();
    void apply_time_derivative();
    double compute_block_dt(double cfl);
    void reconstruct();
    void apply_bcs();

    // configuration settings
    void set_flux_calculator(FluxCalculators flux_calc);
    void set_gas_model(GasModel & gas_model) {_gas_model = &gas_model;}

    // getter methods
    std::vector<Cell> & cells();
    std::vector<Cell> valid_cells();
    std::vector<Vertex> & vertices();
    std::vector<Interface> & interfaces() {return this->_interfaces;}
    std::vector<BoundaryCondition> & bcs() {return _bcs;}
    unsigned int id() const {return this->_id;}

    // get pointers to the data
    Cell * cells_ptr() {return this->_cells.data();}
    Interface * interfaces_ptr() {return this->_interfaces.data();}
    Vertex * vertices_ptr() {return this->_vertices.data();}

    // number of cells/interfaces
    unsigned int number_cells() {return this->_number_valid_cells;}
    unsigned int number_ghost_cells() {return this->_number_ghost_cells;}
    // unsigned int number_ghost_cells() {return this->_ghost_cells.size();}
    unsigned int number_interfaces() {return this->_interfaces.size();}
    unsigned int number_vertices() {return this->_vertices.size();}

private:
    // Collection of cells ghost cells come at the end 
    // of the array
    int _number_valid_cells;
    int _number_ghost_cells;
    std::vector<Cell> _cells;

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

    // id of the fluid block
    unsigned int _id;

    // a local copy of the gas model
    GasModel * _gas_model = nullptr;

    // the flux calculator to use
    flux_calculator _flux_calculator = nullptr; 
};


#endif // __FLUIDBLOCK_H_
