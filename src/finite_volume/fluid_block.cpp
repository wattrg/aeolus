#include "fluid_block.h"

FluidBlock::FluidBlock(const char * file_name) {
    this->fb_config = GlobalConfig();
    this->_grid_io = new GridIO(GridFormat::su2, GridFormat::none);
    this->_grid_io->read_grid(file_name, *this);
    
    // set up the boundary conditions
}

void FluidBlock::fill_function(std::function<FlowState(double, double, double)> &func){
    for (Cell * cell : this->_cells) {
        Vector3 pos = cell->get_pos();
        FlowState fs = func(pos.x, pos.y, pos.z);
        cell->fs.copy(fs);
    }
}

const std::vector<Cell *> & FluidBlock::cells() const{
    return this->_cells;
}

Interface * FluidBlock::_find_interface(std::vector<Vertex *> vertices){
    // Return a pointer to the interface with verteices `vertices`
    for (Interface * interface : this->_interfaces){
        if (interface->is(vertices)) return interface;
    }
    return nullptr;
}

Interface * FluidBlock::_add_interface(std::vector<Vertex *> vertices){
    // loop through all the interfaces we have so far, checking
    // if the this new one is already in our collection of them
    for (Interface * interface : this->_interfaces){
        if (interface->is(vertices)){
            //  the interface already exists, so we'll return
            // a reference to the interface
            return interface;
        }
    }
    // the interface doesn't exist, so we'll create a new one,
    // add it to the list, and return a reference to it
    Interface * interface = new Interface(vertices, this->fb_config);
    this->_interfaces.push_back(interface);
    return interface;
}

std::string FluidBlock::to_string() {
    std::string str = "FluidBlock(";
    str.append("n_interfaces = ");
    str.append(std::to_string(this->_interfaces.size()));
    str.append(", n_cells = ");
    str.append(std::to_string(this->_cells.size()));
    str.append(", cells = [");
    for (Cell * cell : this->_cells){
        str.append(cell->to_string());
        str.append(",\n\n");
    }
    str.append(")");
    return str;
}

std::ostream& operator << (std::ostream& os, const FluidBlock fluid_block){
    os << "FluidBlock(";
    os << "n_interfaces = " << fluid_block._interfaces.size() << ", ";
    for (Cell * cell : fluid_block._cells){
        os << *cell << ", ";
    }
    os << ")\n";
    return os;
}

void FluidBlock::_print_interfaces(){
    for (Interface * interface : this->_interfaces){
        std::cout << *interface << "\n";
    }
}
