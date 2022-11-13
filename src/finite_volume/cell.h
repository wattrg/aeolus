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
    CellFace(Interface & interface, bool outwards) : interface(interface.id()), outwards(outwards){}

    // the interface
    int interface;

    // if this interface points inwards from the cell or not
    bool outwards;
};

class Cell {
public:
    Cell () {};
    Cell(Interface & face, int id, bool valid=true);
    Cell(Grid::Cell & grid_cell, std::vector<Interface>&);

    // the flow state in the cell centre
    // FlowState fs;

    // the conserved quantities
    // ConservedQuantity conserved_quantities;

    // the residual
    // ConservedQuantity residual;


    // compute the residual for a cell, assuming the fluxes have been calculated
    // void compute_time_derivative(Interface *);

    // volume of the cell
    double volume() const;

    const CellFace *interfaces() const {return _interfaces;}
    int number_interfaces() const {return _number_interfaces;}

    // return the position of the cell
    Vector3 & get_pos();

    // return if the cell is a valid cell (as opposed to ghost cell)
    bool is_valid() { return _valid_cell; }

    std::string to_string () const;
    friend std::ostream& operator << (std::ostream& os, const Cell gs);

    // give out some read only info about the vertices
    unsigned int number_vertices() const;
    int *vertices();

    // get info about the shape of the cell
    Grid::CellShape get_shape() const;

    unsigned int id() const {return _id;}

    // used for computing maximum allowable time step
    // double compute_local_timestep(double cfl, Interface *);

private:
    // the interfaces surrounding the cell
    int _number_interfaces = 0;
    CellFace _interfaces[4];

    // the vertices of the cell
    int _number_vertices = 0;
    int _vertices[4];

    // position of the cell centre
    Vector3 _pos;

    // Cell shape
    Grid::CellShape _shape;

    // keep track of if the cell is a valid cell
    bool _valid_cell = false;

    // cell volume
    double _volume = std::nan("");

    int _id = -1;
};

#endif // __CELL_H_
