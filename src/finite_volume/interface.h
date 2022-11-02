#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include "conserved_quantities.h"
#include "flux_calc.h"
#include "../gas/flow_state.h"
#include "../util/vector3.h"
#include "vertex.h"
#include "../grid/grid_interface.h"
#include <vector>
#include <cmath>

class Cell;
class Simulation;

typedef void (*flux_calculator)(FlowState&, FlowState&, ConservedQuantity&);

class Interface {
public:
    Interface() {};
    Interface(Grid::Interface & grid_face);
    ~Interface();

    // area of the interface
    double area() const;


    // return vertices
    // std::vector<Vertex *> & vertices();

    // attach a cell to the interface. This deals with left vs right cells
    // Returns true if the interface is facing the cell
    // (i.e. the cell is to the left of the interface), or false if the interface
    // is facing away from the cell (the cell is to the right of the interface)
    bool attach_cell(Cell & cell);

    // attach cells to the left or right of the interface
    void attach_cell_left(Cell & cell);
    void attach_cell_right(Cell & cell);

    // Return pointers to cells attached to the interface
    int get_left_cell();
    int get_right_cell();
    
    void copy_left_flow_state(FlowState & fs);
    void copy_right_flow_state(FlowState & fs);

    // Mark the interface as being on the boundary
    void mark_on_boundary();
    bool is_on_boundary() const;

    friend std::ostream& operator << (std::ostream& os, const Interface interface);

    // compute the flux across this interface
    // Assumes `_left` and `_right` are correctly set
    void compute_flux(flux_calculator) ;
    ConservedQuantity & flux();

    // Return the ID of the valid cell. Will throw an error if the cells
    // on both sides of the interface are valid
    int get_valid_cell();

    // Return the ID of the ghost cell. Will throw an error if
    // neither cell is a ghost cell
    int get_ghost_cell();

    Vector3 & n() { return this->_norm; }
    Vector3 & t1() { return this->_tan1; }
    Vector3 & t2() {return this->_tan2; }

    int id() const {return _id;}

private:
    // The left and right flow states to use to compute the 
    // fluxes. These states are possibly the result of reconstruction
    FlowState _left;
    FlowState _right;

    // the flux on the cell
    ConservedQuantity _flux;

    //    position of the centre of the interface
    Vector3 _pos;

    // direction normal to the interface
    Vector3 _norm;

    // the two directions tangent to the interface
    Vector3 _tan1, _tan2;

    short _number_vertices = 0;
    int _vertices[2];

    // area
    double _area = std::nan("");
    void _compute_area();

    // cell to the left
    int _left_cell_id = -1;
    bool _left_cell_valid = false;

    // cell to the right
    int _right_cell_id = -1;
    bool _right_cell_valid = false;


    // store if the interface is on a boundary
    bool _is_on_boundary = false;

    // rotate left and right flow states to interface reference frame
    void _transform_flowstate_to_local_frame();

    // rotate left and right flow states to global frame
    void _transform_flowstate_to_global_frame();

    // rotate flux to global frame
    void _transform_flux_to_global_frame();

    unsigned int _id = -1;

    unsigned short _dim;
};

#endif // __INTERFACE_H_
