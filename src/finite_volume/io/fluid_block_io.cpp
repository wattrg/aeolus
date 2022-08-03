#include "fluid_block_io.h"
#include "vtk.h"

FluidBlockIO::FluidBlockIO(FluidBlockFormats::FluidBlockFormat input_fmt,
                           FluidBlockFormats::FluidBlockFormat output_fmt) {
    switch (input_fmt) {
        case FluidBlockFormats::vtk:
            _reader = new VTKReader();
            break;
        case FluidBlockFormats::none:
            break;
    }

    switch (output_fmt) {
        case FluidBlockFormats::vtk:
            _writer = new VTKWriter();
            break;
        case FluidBlockFormats::none:
            break;
    }
}

FluidBlockIO::~FluidBlockIO(){
    delete _reader;
    delete _writer;
}

FluidBlockWriter::~FluidBlockWriter() {}
FluidBlockReader::~FluidBlockReader() {}

void FluidBlockIO::read_fluid_block(const char & file_name, FluidBlock & fb) {
    if (this->_reader) {
        this->_reader->read_fluid_block(file_name, fb);
    }
}

void FluidBlockIO::write_fluid_block(const char & file_name, FluidBlock & fb) {
    if (this->_writer){
        this->_writer->write_fluid_block(file_name, fb);
    }
}

