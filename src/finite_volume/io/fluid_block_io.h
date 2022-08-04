#ifndef __FLUID_BLOCK_IO_H_
#define __FLUID_BLOCK_IO_H_

#include <vector>
#include "../fluid_block.h"
#include "accessor.h"

// forward declaration of FluidBlock
class FluidBlock;
class Accessor;

// Define the formats for fluid block input/output
namespace FluidBlockFormats {
enum FluidBlockFormat {
    vtk, none,   
};
}


// handles writing a fluid block to a file
class FluidBlockWriter {
public:
    virtual ~FluidBlockWriter() = 0;
    virtual void write_fluid_block(const char & file_name, const FluidBlock & fb) = 0;

    void register_accessor(Accessor * accessor);

private:
    std::vector<Accessor *> _variables;
};




// Handles reading a fluid block in from a file
// The fluid block reader assumes that certain variables are defined
class FluidBlockReader{
public:
    virtual ~FluidBlockReader() = 0;
    virtual void read_fluid_block(const char & file_name, const FluidBlock & fb) = 0;
};

class FluidBlockIO {
public:
    FluidBlockIO(FluidBlockFormats::FluidBlockFormat input_fmt, 
                 FluidBlockFormats::FluidBlockFormat output_fmt);
    ~FluidBlockIO();

    void read_fluid_block(const char & file_name,  FluidBlock & fb);
    void write_fluid_block(const char & file_name, FluidBlock & fb);

private:
    FluidBlockWriter * _writer;
    FluidBlockReader * _reader;
};

#endif
