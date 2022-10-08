#ifndef __GRID_IO_H_
#define __GRID_IO_H_

#include "grid_util.h"
#include "grid.h"
#include <fstream>
#include <map>

// forward declarations
namespace Grid {
    class Grid;
    class Interface;
    class Cell;
    enum class ElementShape;
    struct Element;
}

// Interface for specific implementations of reading a grid
class GridInput{
public:
    virtual ~GridInput() {};
    virtual void read_grid(std::string file_name) {};
    std::vector<Grid::Vertex *> vertices() {return _vertices;}
    std::vector<Grid::Interface *> interfaces() {return _interfaces;}
    std::vector<Grid::Cell *> cells() {return _cells;}
    std::map<std::string, std::vector<Grid::Interface *>> bcs() {return _bcs;}
    unsigned int number_boundaries() const {return _number_boundaries;}

protected:
    // storage for the geometry elements
    std::vector<Grid::Vertex *> _vertices;
    std::vector<Grid::Interface *> _interfaces;
    std::vector<Grid::Cell *> _cells;
    
    // boundary conditions
    unsigned int _number_boundaries;
    std::map<std::string, std::vector<Grid::Interface *>> _bcs;

    // Add an interface to the fluid block if it doesn't already exist
    Grid::Interface * _add_interface(std::vector<Grid::Vertex *> vertices);

    // Find the interface with specified vertices.
    // Return null pointer if the interface doesn't exist
    Grid::Interface * _find_interface(std::vector<Grid::Vertex *> vertices);
};

class Su2GridInput : public GridInput {
    void read_grid(std::string file_name);
};

// Interface for specific implementations of writing a grid
class GridOutput {
public:
    virtual ~GridOutput() = 0;
    virtual void write_grid(std::string file_name, Grid::Grid & grid) = 0;
};

// Public interface to handling grid input/output
class GridIO {
public:
    ~GridIO();
    GridIO();

    // read a grid from file
    void read_grid(std::string file_name, Grid::Grid & grid);

    // write a grid to file
    void write_grid(std::string file_name, Grid::Grid & grid);


private:
    GridInput * _grid_input = nullptr;
    GridOutput * _grid_output = nullptr; 
};

// some useful things for reading grids
struct Element {

    Grid::ElementShape shape;
    std::vector<int> vertices;

    Element(Grid::ElementShape shape, std::vector<int> vertices)
        : shape(shape), vertices(vertices)
    {};
};

Grid::ElementShape int_to_element_shape(int shape);
int element_shape_to_number_vertices(Grid::ElementShape shape);
std::string read_string(std::string str);
int read_integer(std::string str);
std::string trim_whitespace(std::string str);
Grid::Element read_element(std::string line);

#endif
