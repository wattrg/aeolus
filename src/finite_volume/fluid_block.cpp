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
    if (_grid_io) delete _grid_io;
}

FluidBlock::FluidBlock(const char * file_name, Simulation & config, unsigned int id) :
    fb_config(config),
    _id(id)
{
    this->_grid_io = new GridIO(GridFormat::su2, GridFormat::none);
    this->_grid_io->read_grid(file_name, *this);
    
    // set up the boundary conditions
    for (Interface * face : this->_interfaces){
        if (face->is_on_boundary()){
            // create a ghost cell
            Cell * left;
            Cell * right;
            left = face->get_left_cell();
            right = face->get_right_cell();
            Cell * cell = new Cell(face, false);
            this->_ghost_cells.push_back(cell); 
            if (!left && right){
                face->attach_cell_left(*cell); 
            }
            else if (left && !right){
                face->attach_cell_right(*cell);    
            }
            else { 
                throw std::runtime_error("It seems a boundary interface has two or no valid cells attached to it");
            }

            // add cell and interface to the relevant boundary condition
        }
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
                          std::vector<BoundaryCondition *> bcs){
    this->_vertices = vertices;
    this->_interfaces = interfaces;
    this->_cells = cells;
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
