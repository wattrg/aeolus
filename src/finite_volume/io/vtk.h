#ifndef __VTK_H_
#define __VTK_H_

#include "fluid_block_io.h"

class VTKWriter : public FluidBlockWriter {
public:
    ~VTKWriter();
    void add_cell_data(std::string name, 
                       std::vector<double>, 
                       unsigned int number_components);
    void write_fluid_block(const char & file_name, const FluidBlock & fb) ;
};

class VTKReader : public FluidBlockReader {
public:
    ~VTKReader();
    void read_fluid_block(const char & file_name, FluidBlock & fb);
};

#endif
