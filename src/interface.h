#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include "cell.h"
#include "flow_state.h"
#include "vector.h"
#include "conserved_quantities.h"
#include "vertex.h"
#include <vector>
#include <cmath>

class Cell;

enum Side { left, right, centre };

class Interface {
public:
    Interface(std::vector<Vertex *> vertices);
    ~Interface();

    // check if two vertices are the same
    bool is(Interface & other);
    bool is(std::vector<Vertex *> & vertices);

    // Check if `vertex` is in an interface
    bool has_vertex(Vertex & other_vertex);

    // attach a cell to the interface. This deals with left vs right cells
    // Returns true if the interface is facing the cell
    // (i.e. the cell is to the left of the interface), or false if the interface
    // is facing away from the cell (the cell is to the right of the interface)
    bool attach_cell(Cell & cell);

    // Return reference to cells attached to the interface
    Cell & get_left_cell();
    Cell & get_right_cell();

    // Mark the interface as being on the boundary
    void mark_on_boundary(std::string tag);

    friend std::ostream& operator << (std::ostream& os, const Interface interface);

    // compute the flux across this interface
    // does not perform reconstruction. Assumes `_left` and `_right` are
    // correctly set
    void compute_flux();

private:
    //  Vertices on the end of the interface
    std::vector<Vertex *> _vertices;

    //    position of the centre of the interface
    Vector3 _pos;

    // direction normal to the interface
    Vector3 _norm;

    // the two directions tangent to the interface
    Vector3 _tan1, _tan2;

    // cell to the left
    Cell * _left_cell = nullptr;

    // cell to the right
    Cell * _right_cell = nullptr;

    // The left and right flow states to use to compute the 
    // fluxes. These states are possibly the result of reconstruction
    FlowState * _left;
    FlowState * _right;

    // the flux on the cell
    ConservedQuantity _flux;

    // store if the interface is on a boundary
    bool _is_on_boundary = false;
    std::string _boundary_tag = "";

    // Figure out if a cell is
    Side _compute_side(Cell & cell);

    // Figure out if a point is on the left or right of the interface
    Side _compute_side(Vector3 & point);

    // pointer to flux calculator function. The actual function
    // takes the left and right `FlowState`, and fills in the 
    // ConservedQuantity flux value.
    void (*_compute_flux)(FlowState&, FlowState&, ConservedQuantity&);

    // rotate left and right flow states to interface reference frame
    void _transform_flowstate_to_local_frame();

    // rotate left and right flow states to global frame
    void _transform_flowstate_to_global_frame();

    // rotate flux to global frame
    void _transform_flux_to_global_frame();
};

#endif // __INTERFACE_H_
