#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include "cell.h"
#include "flow_state.h"
#include "vector.h"
#include "conserved_quantities.h"
#include "vertex.h"
#include <vector>

class Cell;

class Interface {
public:
    Interface(std::vector<Vertex> vertices);

    // check if two vertices are the same
    bool is(Interface & other);

    bool is(std::vector<Vertex> & vertices);

    // Check if `vertex` is in an interface
    bool has_vertex(Vertex & other_vertex);

    friend std::ostream& operator << (std::ostream& os, const Interface interface);

private:
    //  Vertices on the end of the interface
    std::vector<Vertex> _vertices;

    //    position of the centre of the interface
    Vector2 _pos;

    // direction of the interface
    Vector2 _dir;

    // cells to the left
    Cell * _left[2];

    // cells to the right
    Cell * _right[2];

    // the flux on the cell
    ConservedQuantity _flux;
};

#endif // __INTERFACE_H_
