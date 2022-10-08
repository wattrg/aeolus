#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include "../util/array.h"
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

class Interface {
public:
    //Interface(std::vector<Vertex *> vertices, 
    //          Simulation & config, 
    //          unsigned int id,
    //          FluxCalculators flux_calc);
    Interface(Grid::Interface & grid_face, std::vector<Vertex> & vertices);
    ~Interface();

    // area of the interface
    double area() const;

    // check if two vertices are the same
    bool is(Interface & other);
    bool is(std::vector<Vertex *> & vertices);

    // Check if `vertex` is in an interface
    bool has_vertex(Vertex & other_vertex);

    // return vertices
    std::vector<Vertex *> & vertices();

    // attach a cell to the interface. This deals with left vs right cells
    // Returns true if the interface is facing the cell
    // (i.e. the cell is to the left of the interface), or false if the interface
    // is facing away from the cell (the cell is to the right of the interface)
    bool attach_cell(Cell & cell);

    // attach cells to the left or right of the interface
    void attach_cell_left(Cell & cell);
    void attach_cell_right(Cell & cell);

    // Return pointers to cells attached to the interface
    Cell * get_left_cell();
    Cell * get_right_cell();
    
    void copy_left_flow_state(FlowState & fs);
    void copy_right_flow_state(FlowState & fs);

    // Mark the interface as being on the boundary
    void mark_on_boundary(std::string tag);
    bool is_on_boundary() const;

    friend std::ostream& operator << (std::ostream& os, const Interface interface);

    // compute the flux across this interface
    // does not perform reconstruction. Assumes `_left` and `_right` are
    // correctly set
    void compute_flux();
    ConservedQuantity & flux();
    void set_flux_calculator(FluxCalculators flux_calculator);

    // Return a reference to the valid cell. Will throw an error if the cells
    // on both sides of the interface are valid
    Cell & get_valid_cell();

    Vector3 & n() { return this->_norm; }
    Vector3 & t1() { return this->_tan1; }
    Vector3 & t2() {return this->_tan2; }

    int id() const;

private:
    //  Vertices on the end of the interface
    std::vector<Vertex *> _vertices;

    //    position of the centre of the interface
    Vector3 _pos;

    // area
    double _area = std::nan("");
    void _compute_area();

    // direction normal to the interface
    Vector3 _norm;

    // the two directions tangent to the interface
    Vector3 _tan1, _tan2;

    // cell to the left
    Cell * _left_cell = nullptr;
    int _left_cell_id = -1;

    // cell to the right
    Cell * _right_cell = nullptr;
    int _right_cell_id = -1;

    // The left and right flow states to use to compute the 
    // fluxes. These states are possibly the result of reconstruction
    FlowState _left;
    FlowState _right;

    // the flux on the cell
    ConservedQuantity _flux;

    // store if the interface is on a boundary
    bool _is_on_boundary = false;
    std::string _boundary_tag = "";

    // pointer to flux calculator function. The actual function
    // takes the left and right `FlowState`, and fills in the 
    // ConservedQuantity flux value.
    void (*_compute_flux)(FlowState&, FlowState&, ConservedQuantity&) = nullptr;

    // rotate left and right flow states to interface reference frame
    void _transform_flowstate_to_local_frame();

    // rotate left and right flow states to global frame
    void _transform_flowstate_to_global_frame();

    // rotate flux to global frame
    void _transform_flux_to_global_frame();

    unsigned int _id;

    unsigned short _dim;
};

#endif // __INTERFACE_H_
