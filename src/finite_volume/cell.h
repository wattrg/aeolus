#ifndef __CELL_H_
#define __CELL_H_

#include "../util/vector.h"
#include "../gas/flow_state.h"
#include "interface.h"
#include "vertex.h"
#include <vector>

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
    Cell(std::vector<Vertex*> verticies, std::vector<Interface*>);

    // the flow state in the cell centre
    FlowState fs;

    // the conserved quantities
    ConservedQuantity _conserved_quantities;

    // return the position of the cell
    Vector3 & get_pos();

    // return if the cell is a valid cell (as opposed to ghost cell)
    bool is_valid() { return _valid_cell; }

    std::string to_string () const;
    friend std::ostream& operator << (std::ostream& os, const Cell gs);

    // give out some read only info about the vertices
    const unsigned int number_vertices() const;
    const std::vector<Vertex *> & vertices() const;

    // get info about the shape of the cell
    const CellShape::CellShape get_shape() const;

    // compute the residual for a cell, assuming the fluxes have been calculated
    void compute_residual();

private:
    // the interfaces surrounding the cell
    std::vector<CellFace> _interfaces;

    // the vertices of the cell
    std::vector<Vertex *> _vertices;

    // position of the cell centre
    Vector3 _pos;

    // Cell shape
    CellShape::CellShape _shape;

    // keep track of if the cell is a valid cell
    bool _valid_cell;


    friend class InternalCopy;
    friend class ReflectNormal;
    friend class ReflectTangential;
};

#endif // __CELL_H_
