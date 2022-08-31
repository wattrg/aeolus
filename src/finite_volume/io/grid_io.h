#ifndef __FLUIDBLOCK_IO_H_
#define __FLUIDBLOCK_IO_H_

#include "../fluid_block.h"
#include "../config.h"
#include "../boundary_conditions/boundary_condition.h"
#include <fstream>
#include <map>

class Interface;
class FluidBlock;
class Cell;
class Simulation;
class BoundaryCondition;

namespace ElementShape{ enum ElementShape : unsigned short; };

namespace GridFormat {
enum GridFormats{
    su2,
    none,
};
}

// Interface for specific implementations of reading a grid
class GridInput{
public:
    virtual ~GridInput() = 0;
    virtual void read_grid(const char * file_name, FluidBlock & fb, 
                            std::map<std::string, BoundaryCondition> & bc_map) = 0;
    std::vector<Vertex *> vertices() {return _vertices;}
    std::vector<Interface *> interfaces() {return _interfaces;}
    std::vector<Cell *> cells() {return _cells;}
    std::vector<Cell *> ghost_cells() {return _ghost_cells;}
    std::vector<BoundaryCondition *> boundary_conditions() {return _bcs;}
    unsigned int number_boundaries() const {return _number_boundaries;}

protected:
    // storage for the geometry elements
    std::vector<Vertex *> _vertices;
    std::vector<Interface *> _interfaces;
    std::vector<Cell *> _cells;
    
    // boundary conditions
    unsigned int _number_boundaries;
    std::vector<BoundaryCondition *> _bcs;
    std::vector<Cell *> _ghost_cells;

    // Add an interface to the fluid block if it doesn't already exist
    Interface * _add_interface(std::vector<Vertex *> vertices, Simulation & config);

    // Find the interface with specified vertices.
    // Return null pointer if the interface doesn't exist
    Interface * _find_interface(std::vector<Vertex *> vertices);
};

// Interface for specific implementations of writing a grid
class GridOutput {
public:
    virtual ~GridOutput() = 0;
    virtual void write_grid(const char * file_name, FluidBlock & fb) = 0;
};

// Public interface to handling fluid block input/output
class GridIO {
public:
    ~GridIO();
    GridIO(GridFormat::GridFormats input, GridFormat::GridFormats output);

    // read a grid from file
    void read_grid(const char * file_name, FluidBlock &fb, std::map<std::string, BoundaryCondition>& bc_map);

    // write a grid to file
    void write_grid(const char * file_name, FluidBlock &fb);


private:
    GridInput * _grid_input = nullptr;
    GridOutput * _grid_output = nullptr; 
};

// some useful things for reading grids
struct Element {

    ElementShape::ElementShape shape;
    std::vector<int> vertices;

    Element(ElementShape::ElementShape shape, std::vector<int> vertices)
        : shape(shape), vertices(vertices)
    {};
};

ElementShape::ElementShape int_to_element_shape(int shape);
int element_shape_to_number_vertices(ElementShape::ElementShape shape);
std::string read_string(std::string str);
int read_integer(std::string str);
std::string trim_whitespace(std::string str);
Element read_element(std::string line);

#endif
