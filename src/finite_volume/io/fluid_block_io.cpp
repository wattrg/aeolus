#include "fluid_block_io.h"
#include "vtk.h"

FluidBlockIO::FluidBlockIO() {
    _writer = new VTKWriter();
    _reader = nullptr;


}

FluidBlockIO::FluidBlockIO(FluidBlockFormats::FluidBlockFormat input_fmt,
                           FluidBlockFormats::FluidBlockFormat output_fmt) {
    switch (input_fmt) {
        case FluidBlockFormats::vtk:
            //_reader = new VTKReader();
            throw std::runtime_error("reading from vtk not supported yet");
            break;
        case FluidBlockFormats::none:
            _reader = nullptr;
            break;
        default:
            throw std::runtime_error("Unknown fluid block format");
    }

    switch (output_fmt) {
        case FluidBlockFormats::vtk:
            _writer = new VTKWriter();
            break;
        case FluidBlockFormats::none:
            _writer = nullptr;
            break;
        default:
            throw std::runtime_error("Unknown fluid block format");
    }
}

FluidBlockIO::~FluidBlockIO(){
    if (this->_writer){
        delete _writer;
    }
    if (this->_reader){
        delete _reader;
    }
}

FluidBlockWriter::FluidBlockWriter(){
   // default things to output
   this->add_variable(Accessor("pressure", &access_pressure, 1));
   this->add_variable(Accessor("density", &access_density, 1));
   this->add_variable(Accessor("temperature", &access_temperature, 1));
   this->add_variable(Accessor("velocity", &access_velocity, 3));
}

void FluidBlockWriter::add_variable(Accessor accessor){
    this->_variable_accessors.push_back(accessor);
}

FluidBlockWriter::~FluidBlockWriter() {}
FluidBlockReader::~FluidBlockReader() {}

void FluidBlockIO::read_fluid_block(const char * file_name, FluidBlock & fb) {
    if (this->_reader) {
        this->_reader->read_fluid_block(*file_name, fb);
    }
}

void FluidBlockIO::write_fluid_block(const char * file_name, const FluidBlock & fb) {
    if (this->_writer){
        this->_writer->write_fluid_block(*file_name, fb);
    }
    else {
        throw std::runtime_error("No fluid block writer configured");
    }
}


