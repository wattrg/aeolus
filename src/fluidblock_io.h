#ifndef __FLUIDBLOCK_IO_H_
#define __FLUIDBLOCK_IO_H_

#include "fluid_block.h"

// Interface for specific implementations of reading a grid
class GridInput{
public:
    virtual void read_grid(const char * file_name, FluidBlock & fb) = 0;
};

// Interface for specific implementations of writing a grid
class GridOutput {
public:
    virtual void write_grid(const char & file_name) = 0;
};

// Public interface to handling fluid block input/output
class FluidBlockIO {
public:
    // read a grid from file
    void read_grid(const char * file_name);

    // write a grid to file
    void write_grid(const char * file_name);

    // write a fluid block to file
    void write_fluidblock(const char * file_name);

private:
    GridInput * _grid_input;
    GridOutput * _grid_output; 
};

#endif
