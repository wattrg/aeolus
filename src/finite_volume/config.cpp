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

void Simulation::add_fluid_block(
    const char * file_name, 
    std::function<FlowState(double, double, double)> & fill_func,
    std::map<std::string, BoundaryCondition> & bc_map)
{
    unsigned int id = this->_fluid_blocks.size();
    FluidBlock * fb = new FluidBlock(file_name, *this, id, bc_map);
    fb->fill(fill_func);
    this->_fluid_blocks.push_back(fb);
}

void Simulation::add_fluid_block(
    const char * file_name, 
    FlowState & fs,
    std::map<std::string, BoundaryCondition> & bc_map)
{
    unsigned int id = this->_fluid_blocks.size();
    FluidBlock * fb = new FluidBlock(file_name, *this, id, bc_map);
    fb->fill(fs);
    this->_fluid_blocks.push_back(fb);
}

std::vector<FluidBlock *> & Simulation::fluid_blocks() {
    return this->_fluid_blocks;
}

void Simulation::write_fluid_blocks(){
    for (const FluidBlock * fb : this->_fluid_blocks){
        std::string file_name = "blk" + std::to_string(fb->id()) + "t" + std::to_string(this->_time_index) + ".vtu";
        mkdir("flow", 0777);
        this->_fluid_block_io->write_fluid_block(file_name.c_str(), *fb, this->_time);
    }
    this->_time_index += 1;
}

const unsigned short Simulation::dimensions() const {return this->_dimensions;}

void Simulation::set_dimensions(unsigned short number_dim) {
    this->_dimensions = number_dim;
}

const Solver & Simulation::solver() const {
    if (this->_solvers.size() == 0)
        throw std::runtime_error("No solvers initialised");
    return *this->_solvers[_solver_idx];
}

void Simulation::run() {
    for (Solver * solver : this->_solvers){
        
        solver->solve();
        this->_solver_idx += 1;
    }
}

void Simulation::add_solver(Solver & solver){
    this->_solvers.push_back(&solver);
}
