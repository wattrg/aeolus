#ifndef __CONFIG_H
#define __CONFIG_H

#include "io/fluid_block_io.h"
#include "fluid_block.h"
#include "boundary_conditions/boundary_condition.h"
#include <sys/stat.h>
#include <map>


class FluidBlock;
class FluidBlockIO;

class Simulation{

public:
    Simulation();
    ~Simulation();
    void add_fluid_block(const char *, std::map<std::string, BoundaryCondition> &);
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
