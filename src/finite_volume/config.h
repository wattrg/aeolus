#ifndef __CONFIG_H
#define __CONFIG_H

#include "fluid_block.h"

class FluidBlock;

class GlobalConfig{
public:
    void add_fluid_block(const char *);
    const unsigned short dimensions() const;
    void set_dimensions(unsigned short number_dim);
    const unsigned int max_step() const;
    void set_max_step(unsigned int max_step);
    std::vector<FluidBlock *> & fluid_blocks();

private:
    unsigned int _max_step = 0;
    unsigned short _dimensions = 2;
    std::vector<FluidBlock *> _fluid_blocks;
};

#endif
