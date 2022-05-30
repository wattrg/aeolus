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
    Interface(std::vector<Vertex> vertices);

    // check if two vertices are the same
    bool is(Interface & other);

    bool is(std::vector<Vertex> & vertices);

    // Check if `vertex` is in an interface
    bool has_vertex(Vertex & other_vertex);

    // attach a cell to the interface. This deals with left vs right cells
    void attach_cell(Cell & cell);

    friend std::ostream& operator << (std::ostream& os, const Interface interface);

private:
    //  Vertices on the end of the interface
    std::vector<Vertex> _vertices;

    //    position of the centre of the interface
    Vector2 _pos;

    // direction of the interface
    Vector2 _dir;

    // cell to the left
    Cell * _left = nullptr;

    // cell to the right
    Cell * _right = nullptr;

    // the flux on the cell
    ConservedQuantity _flux;

    // Figure out if a cell is on the left or right of the interface
    Side _compute_side(Cell & cell);

    // Figure out if a point is on the left or right of the interface
    Side _compute_side(Vector2 & point);
};

#endif // __INTERFACE_H_
