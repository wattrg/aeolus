#ifndef __GRID_CELL_H_
#define __GRID_CELL_H_

#include <vector>
#include "grid_vertex.h"
#include "grid_interface.h"
#include "grid_util.h"

namespace Grid {

class Interface;
class Vertex;
enum class CellShape;

struct CellFace {
    CellFace(Interface & interface, bool outwards)
        : interface(&interface), outwards(outwards) {}

    Interface * interface;
    bool outwards;
};

class Cell{
public:
    Cell(std::vector<Vertex *> vertices,
         std::vector<Interface *> interfaces,
         unsigned int id);

    Vector3 & get_pos() { return _pos; }
    std::vector<Vertex *> & vertices() {return _vertices;}
    std::vector<Vector3> & vertex_positions() const; 
    std::vector<CellFace> & interfaces() {return _interfaces;}
    CellShape get_shape() const {return _shape;}
    int id() const {return _id;}
    double volume() const {return this->_volume;}
    unsigned int number_vertices() const { return this->_vertices.size(); }

    std::string to_string() const;

private:
    std::vector<Vertex *> _vertices;
    std::vector<CellFace> _interfaces;

    Vector3 _pos;
    CellShape _shape;
    int _id = -1;
    double _volume;

    void _compute_volume();
    void _compute_shape();
};

}

#endif
