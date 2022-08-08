#ifndef __FLUIDBLOCK_IO_H_
#define __FLUIDBLOCK_IO_H_

#include "../fluid_block.h"
#include <fstream>

class FluidBlock;
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
    virtual void read_grid(const char * file_name, FluidBlock & fb) = 0;
    std::vector<Vertex *> vertices() {return _vertices;}
    std::vector<Interface *> interfaces() {return _interfaces;}
    std::vector<Cell *> cells() {return _cells;}

protected:
    // storage for the geometry elements
    std::vector<Vertex *> _vertices;
    std::vector<Interface *> _interfaces;
    std::vector<Cell *> _cells;
    
    // Add an interface to the fluid block if it doesn't already exist
    Interface * _add_interface(std::vector<Vertex *> vertices, GlobalConfig & config);

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
    void read_grid(const char * file_name, FluidBlock &fb);

    // write a grid to file
    void write_grid(const char * file_name, FluidBlock &fb);


private:
    GridInput * _grid_input;
    GridOutput * _grid_output; 

    friend class GridInput;
    friend class GridOutput;
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
Element read_element(std::string line);

#endif
