#ifndef __FLUID_BLOCK_IO_H_
#define __FLUID_BLOCK_IO_H_

#include "../fluid_block.h"

class FluidBlock;

namespace FluidBlockFormats {
enum FluidBlockFormat {
    vtk, none,   
};
}

class FluidBlockWriter {
public:
    virtual void write_fluid_block(const char & file_name, FluidBlock & fb) = 0;
};

class FluidBlockReader{
public:
    virtual void read_fluid_block(const char & file_name, FluidBlock & fb) = 0;
};

class FluidBlockIO {
public:
    FluidBlockIO(FluidBlockFormats::FluidBlockFormat input_fmt, 
                 FluidBlockFormats::FluidBlockFormat output_fmt);

    void read_fluid_block(const char & file_name,  FluidBlock & fb);
    void write_fluid_block(const char & file_name, FluidBlock & fb);

private:
    FluidBlockWriter * _writer;
    FluidBlockReader * _reader;
};

#endif
