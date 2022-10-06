#ifndef __CELL_H_
#define __CELL_H_

#include "conserved_quantities.h"
#include "../grid/grid_cell.h"
#include "../util/volume.h"
#include "../gas/flow_state.h"
#include "interface.h"
#include "config.h"
#include "vertex.h"
#include <vector>
#include <cmath>

class Interface;


// keep track of the interface, as well as if the interface is facing the cell
// or not
struct CellFace {
    CellFace() {}
    CellFace(Interface & interface, bool outwards) : interface(&interface), outwards(outwards){}
    // the interface
    Interface * interface;

    // if this interface points inwards from the cell or not
    bool outwards;
};

class Cell {
public:
    Cell(Interface * face, bool valid=true);
    Cell(Grid::Cell & grid_cell, std::vector<Vertex>&, std::vector<Interface>&);

    // the flow state in the cell centre
    FlowState fs;

    // set the gas model
    void set_gas_model(GasModel * gas_model) {_gas_model = gas_model;}

    // the conserved quantities
    ConservedQuantity conserved_quantities;

    // the residual
    ConservedQuantity residual;

    // encode conserved quantities
    void encode_conserved();

    // decode consered quantities
    void decode_conserved();

public:
    // compute the residual for a cell, assuming the fluxes have been calculated
    void compute_time_derivative();

    // volume of the cell
    double volume() const;

    // return the position of the cell
    Vector3 & get_pos();

    // return if the cell is a valid cell (as opposed to ghost cell)
    bool is_valid() { return _valid_cell; }

    std::string to_string () const;
    friend std::ostream& operator << (std::ostream& os, const Cell gs);

    // give out some read only info about the vertices
    unsigned int number_vertices() const;
    const std::vector<Vertex *> & vertices() const;
    const std::vector<Vector3> & vertex_positions() const;

    // get info about the shape of the cell
    Grid::CellShape get_shape() const;

    unsigned int id() const;

    // used for computing maximum allowable time step
    double compute_local_timestep(double cfl);


private:
    // the interfaces surrounding the cell
    std::vector<CellFace> _interfaces;

    // the vertices of the cell
    std::vector<Vertex *> _vertices;

    // position of the cell centre
    Vector3 _pos;

    // Cell shape
    Grid::CellShape _shape;

    // keep track of if the cell is a valid cell
    bool _valid_cell;

    // gas model
    GasModel * _gas_model = nullptr;

    // cell volume
    double _volume = std::nan("");

    unsigned int _id;

    // the local time step
    bool _lts = false;
    double _dt;

    friend class InternalCopy;
    friend class ReflectNormal;
    friend class ReflectTangential;
};

#endif // __CELL_H_
