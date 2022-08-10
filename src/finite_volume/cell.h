#ifndef __CELL_H_
#define __CELL_H_

#include "conserved_quantities.h"
#include "../util/vector.h"
#include "../util/volume.h"
#include "../gas/flow_state.h"
#include "interface.h"
#include "vertex.h"
#include <vector>
#include <cmath>

class Interface;

namespace CellShape {
    enum CellShape {
        Quad
    };
}

// keep track of the interface, as well as if the interface is facing the cell
// or not
struct CellFace {
    CellFace(Interface & interface, bool inwards) : interface(&interface), inwards(inwards){}
    // the interface
    Interface * interface;

    // if this interface points inwards from the cell or not
    bool inwards;
};

class Cell {
public:
    Cell(Interface * face, bool valid=true);
    Cell(std::vector<Vertex*> verticies, std::vector<Interface*>, unsigned int id, bool valid=true);

    // the flow state in the cell centre
    FlowState fs;

    // the conserved quantities
    ConservedQuantity conserved_quantities;

    // the residual
    ConservedQuantity residual;

public:
    // compute the residual for a cell, assuming the fluxes have been calculated
    void compute_time_derivative();

    // volume of the cell
    const double volume() const;

    // return the position of the cell
    Vector3 & get_pos();

    // return if the cell is a valid cell (as opposed to ghost cell)
    bool is_valid() { return _valid_cell; }

    std::string to_string () const;
    friend std::ostream& operator << (std::ostream& os, const Cell gs);

    // give out some read only info about the vertices
    const unsigned int number_vertices() const;
    const std::vector<Vertex *> & vertices() const;
    const std::vector<Vector3> & vertex_positions() const;

    // get info about the shape of the cell
    const CellShape::CellShape get_shape() const;

    const unsigned int id() const;


private:
    // the interfaces surrounding the cell
    std::vector<CellFace> _interfaces;

    // the vertices of the cell
    std::vector<Vertex *> _vertices;

    // position of the cell centre
    Vector3 _pos;

    // Cell shape
    CellShape::CellShape _shape;
    void _compute_shape();

    // keep track of if the cell is a valid cell
    bool _valid_cell;

    // cell volume
    double _volume = std::nan("");
    void _compute_volume();

    unsigned int _id;

    friend class InternalCopy;
    friend class ReflectNormal;
    friend class ReflectTangential;
};

#endif // __CELL_H_
