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

    //  position of the centre of the interface
    Vector2 pos;

    // direction of the interface
    Vector2 dir;

    // cells to the left
    Cell * left[2];

    // cells to the right
    Cell * right[2];

    // the flux on the cell
    ConservedQuantity flux;

    friend std::ostream& operator << (std::ostream& os, const Interface interface);

private:
    //  Vertices on the end of the interface
    Vertex * _vertices[2];
};

#endif // __INTERFACE_H_
