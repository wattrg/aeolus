#include "config.h"

void GlobalConfig::add_fluid_block(const char * file_name){
    unsigned int id = this->_fluid_blocks.size();
    this->_fluid_blocks.push_back(new FluidBlock(file_name, *this, id));
}

std::vector<FluidBlock *> & GlobalConfig::fluid_blocks() {
    return this->_fluid_blocks;
}

const unsigned short GlobalConfig::dimensions() const {return this->_dimensions;}

void GlobalConfig::set_dimensions(unsigned short number_dim) {
    this->_dimensions = number_dim;
}

const unsigned int GlobalConfig::max_step() const {return this->_max_step;}

void GlobalConfig::set_max_step(unsigned int max_step) {this->_max_step = max_step;}
