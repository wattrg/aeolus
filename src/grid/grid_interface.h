#ifndef __GRID_INTERFACE_H_
#define __GRID_INTERFACE_H_

#include <vector>
#include <string>
#include "grid_vertex.h"
#include "grid_cell.h"
#include "../util/volume.h"

enum class Side {left, right, centre};

namespace Grid {

class Cell;

class Interface{
public:
    Interface(std::vector<Vertex *> vertices, unsigned int id);

    bool is (std::vector<Vertex *> & vertices){
        for (Vertex * other_vertex : vertices){
            for (Vertex * this_vertex : this->_vertices) {
                if (other_vertex != this_vertex) return false;
            }
        }
        return true;
    }

    bool attach_cell(Cell & cell);
    void attach_cell_left(Cell & cell);
    void attach_cell_right(Cell & cell);
    void mark_on_boundary(std::string tag);
    unsigned int id() const {return _id;}
    std::string boundary_tag() {return _boundary_tag;}
    Vector3 norm() const {return _n;}
    Vector3 tan1() const {return _t1;}
    Vector3 tan2() const {return _t2;}
    double area() const { return _area; }

private:
    std::vector<Vertex *> _vertices;
    unsigned int _id;
    Cell * _left_cell = nullptr;
    Cell * _right_cell = nullptr;
    Vector3 _n, _t1, _t2;
    double _area;
    bool _is_on_boundary = false;
    std::string _boundary_tag = "";

    Side _compute_side(Cell & cell);
    Side _compute_side(Vector3 & point);
};

}

#endif
