#include "vtk.h"

// VTK writer
VTKWriter::~VTKWriter() {}

void VTKWriter::write_fluid_block(const char & file_name, const FluidBlock & fb){
    std::runtime_error("Writing vtk files not implemented yet");
}

// VTK reader 
VTKReader::~VTKReader() {}

void VTKReader::read_fluid_block(const char & file_name, FluidBlock & fb){
    std::runtime_error("Reading vtk files not implemented yet");
}
