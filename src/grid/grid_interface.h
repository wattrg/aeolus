#ifndef __GRID_INTERFACE_H_
#define __GRID_INTERFACE_H_

#include <vector>
#include <map>
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
    // Interface(Interface & other);

    bool is (std::vector<Vertex *> & vertices);

    bool attach_cell(Cell & cell);
    void attach_cell_left(Cell & cell);
    void attach_cell_right(Cell & cell);
    Cell * get_left_cell() {return this->_left_cell;}
    Cell * get_right_cell() {return this->_right_cell;}
    int get_left_cell_id();
    int get_right_cell_id();
    void mark_on_boundary(std::string tag);
    unsigned int id() const {return _id;}
    std::string boundary_tag() {return _boundary_tag;}
    Vector3 norm() const {return _n;}
    Vector3 tan1() const {return _t1;}
    Vector3 tan2() const {return _t2;}
    double area() const { return _area; }
    std::string to_string() const;
    std::vector<Vertex *> & vertices() {return _vertices;}
    bool is_on_boundary() const {return _is_on_boundary;}

private:
    std::vector<Vertex *> _vertices;
    unsigned int _id = -1;
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

class InterfaceCollection {
    // Class to handle the construction of interfaces, without repeating
    // the same interface from both sides
public:
    InterfaceCollection() {};
    ~InterfaceCollection() {};

    // add an interface if it doesn't exist. Return pointer to the newly created
    // interface, or pointer to the already existing interface.
    Grid::Interface * add_or_retrieve(std::vector<Grid::Vertex *> vertices);

    // find an interface in the collection. Error if the interface was not present
    Grid::Interface * retrieve(std::vector<Grid::Vertex *> vertices);

    // access interface by its ID
    Grid::Interface * operator[](int id);

    // return vector of interface poniters
    std::vector<Grid::Interface *> interfaces();

private:
    // map hash -> interface
    std::map<std::string, Grid::Interface> _interfaces;
    
    // map interface id -> hash
    std::map<int, std::string> _id_to_hash;
};


#endif
