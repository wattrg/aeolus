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
#include "../grid/grid.h"
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
    void compute_residuals();
    void apply_residuals(double dt);
    double compute_block_dt(double cfl);
    void reconstruct();
    void apply_bcs();
    void encode_conserved(bool gpu=false);
    void decode_conserved(bool gpu=false);

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
    unsigned int number_interfaces() {return this->_interfaces.size();}
    unsigned int number_vertices() {return this->_vertices.size();}

    // Service methods to allow interaction with the fluid block
    // on a cell by cell basis. Probably pretty slow, but convienient
    FlowState get_flow(int i);
    void set_flow(int i, FlowState flow);

private:
    int _number_valid_cells;
    int _number_ghost_cells;
    std::vector<Cell> _cells;

    // Collection of interfaces
    std::vector<Interface> _interfaces;

    // Collection of vertices
    std::vector<Vertex> _vertices;

    // flow state
    std::vector<double> _p;
    std::vector<double> _T;
    std::vector<double> _rho;
    std::vector<double> _u;
    std::vector<double> _a;
    std::vector<double> _vx;
    std::vector<double> _vy;
    std::vector<double> _vz;

    // conserved quantities
    std::vector<double> _mass;
    std::vector<double> _px;
    std::vector<double> _py;
    std::vector<double> _pz;
    std::vector<double> _e;

    // the residuals
    std::vector<double> _dmass_dt;
    std::vector<double> _dpx_dt;
    std::vector<double> _dpy_dt;
    std::vector<double> _dpz_dt;
    std::vector<double> _de_dt;

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
