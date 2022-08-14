#include "fluid_block_io.h"
#include "vtk.h"

FluidBlockWriter::FluidBlockWriter(){
   // default things to output
   this->add_variable("pressure", &access_pressure, 1);
   this->add_variable("density", &access_density, 1);
   this->add_variable("temperature", &access_temperature, 1);
   this->add_variable("velocity", &access_velocity, 3);
   this->add_variable("volume", &access_volume, 1);
}

FluidBlockIO::FluidBlockIO() {
    _writer = new VTKWriter();
    _reader = nullptr;
}

FluidBlockIO::~FluidBlockIO(){
    if (this->_writer) delete this->_writer;
    if (this->_reader) delete this->_reader;
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

void FluidBlockWriter::add_variable(std::string name, 
                                    std::vector<double> (*access_from_cell)(const Cell &),
                                    int number_of_components){
    this->_variable_accessors.push_back(Accessor(name, access_from_cell, number_of_components));
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


