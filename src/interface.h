#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include "cell.h"
#include "flow_state.h"
#include "vector.h"
#include "conserved_quantities.h"
#include "vertex.h"

class Cell;

class Interface {
public:
    Interface(Vertex & start, Vertex & end);

    // return references to the start and end vertices
    Vertex & get_start_vertex() const;
    Vertex & get_end_vertex() const;

    // check if two vertices are the same
    bool is_close(Interface other);

    friend std::ostream& operator << (std::ostream& os, const Interface interface);

private:
    //  Vertices on the end of the interface
    Vertex * _vertices[2];

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
