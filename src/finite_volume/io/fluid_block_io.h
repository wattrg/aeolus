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

template <typename T>
struct GridData{
    GridData(std::string name, std::vector<T> data, unsigned int number_components, std::string type);
    std::string name;
    std::vector<T> data;
    unsigned int number_components;
    std::string type_name;
};

// handles writing a fluid block to a file
class FluidBlockWriter {
public:
    virtual ~FluidBlockWriter() = 0;
    virtual void write_fluid_block(const char & file_name, const FluidBlock & fb) = 0;

    void register_accessor(Accessor * accessor);

protected:
    // one accessor for each flow variable
    std::vector<Accessor *> _variable_accessors;

    // place to keep all the flow data
    std::vector<GridData<double>> _flow_data;
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
    void write_fluid_block(const char & file_name, const FluidBlock & fb);

private:
    FluidBlockWriter * _writer;
    FluidBlockReader * _reader;
};

#endif
