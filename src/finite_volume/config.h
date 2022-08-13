#ifndef __CONFIG_H
#define __CONFIG_H

#include "io/fluid_block_io.h"
#include "fluid_block.h"
#include <sys/stat.h>


class FluidBlock;
class FluidBlockIO;

class Simulation{

public:
    Simulation();
    ~Simulation();
    void add_fluid_block(const char *);
    const unsigned short dimensions() const;
    void set_dimensions(unsigned short number_dim);
    std::vector<FluidBlock *> & fluid_blocks();
    void write_fluid_blocks();

private:
    unsigned short _dimensions = 2;
    std::vector<FluidBlock *> _fluid_blocks;
    FluidBlockIO * _fluid_block_io = nullptr;
};

#endif
