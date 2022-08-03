#ifndef __FLUIDBLOCK_IO_H_
#define __FLUIDBLOCK_IO_H_

#include "../finite_volume/fluid_block.h"

class FluidBlock;

namespace GridFormat {

enum GridFormats{
    su2,
    none,
};

}

// Interface for specific implementations of reading a grid
class GridInput{
public:
    virtual void read_grid(const char * file_name, FluidBlock & fb) = 0;
};

// Interface for specific implementations of writing a grid
class GridOutput {
public:
    virtual void write_grid(const char * file_name, FluidBlock & fb);
};

// Public interface to handling fluid block input/output
class GridIO {
public:
    GridIO(GridInput * input, GridOutput * output);
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

// su2 grid reader
class Su2GridInput : public GridInput {
    void read_grid(const char * file_name, FluidBlock & fb);   
};

#endif
