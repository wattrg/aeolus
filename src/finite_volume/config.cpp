#include "config.h"

Simulation::Simulation(){
    this->_fluid_block_io = new FluidBlockIO();
}


Simulation::~Simulation(){
    for (FluidBlock * fb : this->_fluid_blocks){
        delete fb;
    }
    delete _fluid_block_io;
}

void Simulation::add_fluid_block(const char * file_name, 
        std::map<std::string, BoundaryCondition> & bc_map){
    unsigned int id = this->_fluid_blocks.size();
    this->_fluid_blocks.push_back(new FluidBlock(file_name, *this, id, bc_map));
}

std::vector<FluidBlock *> & Simulation::fluid_blocks() {
    return this->_fluid_blocks;
}

void Simulation::write_fluid_blocks(){
    for (const FluidBlock * fb : this->_fluid_blocks){
        std::string file_name = "flow/blk" + std::to_string(fb->id()) + "t" + std::to_string(this->_time_index) + ".vtu";
        mkdir("flow", 0777);
        this->_fluid_block_io->write_fluid_block(file_name.c_str(), *fb);
    }
    this->_time_index += 1;
}

const unsigned short Simulation::dimensions() const {return this->_dimensions;}

void Simulation::set_dimensions(unsigned short number_dim) {
    this->_dimensions = number_dim;
}

