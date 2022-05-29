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

    // return references to the start and end vertices
    Vertex & get_start_vertex();
    Vertex & get_end_vertex();

    // check if two vertices are the same
    bool is_close(Interface & other);

    bool is_close(std::vector<Vertex> & vertices);

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
