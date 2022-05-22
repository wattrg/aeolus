#ifndef __CELL_H_
#define __CELL_H_

#include "vector.h"
#include "flow_state.h"
#include "interface.h"
#include "vertex.h"
#include <vector>

class Interface;

enum CellShape {
    Quad
};

class Cell {
public:
    Cell(std::vector<Vertex> verticies, std::vector<Interface>);

    // the flow state in the cell centre
    FlowState fs;

    // the position of the cell
    Vector2 pos;

    friend std::ostream& operator << (std::ostream& os, const Cell gs);

private:
    // the interfaces surrounding the cell
    std::vector<Interface> _interfaces;

    // the vertices of the cell
    std::vector<Vertex> _vertices;

    // Cell shape
    CellShape _shape;
};

#endif // __CELL_H_
