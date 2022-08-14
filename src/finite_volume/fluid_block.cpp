#include "fluid_block.h"

FluidBlock::~FluidBlock(){
    for (Cell * cell : this->_cells){
        delete cell;
    }
    for (Cell * cell : this->_ghost_cells){
        delete cell;
    }
    for (Interface * interface : this->_interfaces){
        delete interface;
    }
    for (Vertex * vertex : this->_vertices){
        delete vertex;
    }
    //for (BoundaryCondition * bc : this->_bcs){
    //    delete bc;
    //}
    if (_grid_io) delete _grid_io;
}

FluidBlock::FluidBlock(const char * file_name, Simulation & config, unsigned int id, 
        std::map<std::string, BoundaryCondition> & bc_map) :
    fb_config(config),
    _id(id)
{
    this->_grid_io = new GridIO(GridFormat::su2, GridFormat::none);
    this->_grid_io->read_grid(file_name, *this, bc_map);
}

void FluidBlock::compute_fluxes(){
    for (Interface * interface : this->_interfaces){
        interface->compute_flux(); 
    }
}

void FluidBlock::compute_time_derivatives(){
    for (Cell * cell : this->_cells){
        cell->compute_time_derivative();
    }
}

void FluidBlock::compute_block_dt(){
    double dt = 10000;
    for (Cell * cell : this->_cells){
        dt = std::min(cell->compute_local_timestep(), dt); 
    }
    this->_dt = dt;
}

void FluidBlock::apply_time_derivative(){
    for (Cell * cell : this->_cells){
        ConservedQuantity cq = cell->conserved_quantities;
        for (unsigned int i=0; i < cq.n_conserved(); i++){
            cq.conserved_quantities[i] += cell->residual.conserved_quantities[i] * this->_dt; 
        }
        cell->decode_conserved();
    }
}

void FluidBlock::reconstruct(){
    for (Interface * face : this->_interfaces){
        face->set_left_flow_state(face->get_left_cell()->fs);
        face->set_right_flow_state(face->get_right_cell()->fs);
    }
}

void FluidBlock::fill_function(std::function<FlowState(double, double, double)> &func){
    for (Cell * cell : this->_cells) {
        Vector3 pos = cell->get_pos();
        FlowState fs = func(pos.x, pos.y, pos.z);
        cell->fs.copy(fs);
    }
}

void FluidBlock::set_grid(std::vector<Vertex *> vertices, 
                          std::vector<Interface *> interfaces, 
                          std::vector<Cell *> cells,
                          std::vector<Cell *> ghost_cells,
                          std::vector<BoundaryCondition *> bcs){
    this->_vertices = vertices;
    this->_interfaces = interfaces;
    this->_cells = cells;
    this->_ghost_cells = ghost_cells;
    this->_bcs = bcs;
}


const std::vector<Cell *> & FluidBlock::cells() const{
    return this->_cells;
}

const std::vector<Vertex *> & FluidBlock::vertices() const {
    return this->_vertices;
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
