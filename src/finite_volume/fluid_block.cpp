#include "fluid_block.h"
#include "io/fluid_block_io.h"
#include "config.h"

FluidBlock::~FluidBlock(){
    //for (Cell * cell : this->_cells){
    //    delete cell;
    //}
    //for (Cell * cell : this->_ghost_cells){
    //    delete cell;
    //}
    //for (Interface * interface : this->_interfaces){
    //    delete interface;
    //}
    //for (Vertex * vertex : this->_vertices){
    //    delete vertex;
    //}
    //for (BoundaryCondition * bc : this->_bcs){
    //    delete bc;
    //}
}

FluidBlock::FluidBlock(Grid::Grid & grid, unsigned int id, 
                       std::map<std::string, BoundaryCondition> & bc_map) :
    _id(id)
{
    // read the vertices
    std::vector<Grid::Vertex *> vertices = grid.vertices();
    for (Grid::Vertex * grid_vertex : vertices){
        this->_vertices.push_back(Vertex(*grid_vertex));
    }

    // read the interfaces
    std::vector<Grid::Interface *> interfaces = grid.interfaces();
    for (Grid::Interface * grid_interface : interfaces){
        this->_interfaces.push_back(Interface(*grid_interface, this->_vertices));
    }

    // read the cells
    std::vector<Grid::Cell *> cells = grid.cells();
    for (Grid::Cell * grid_cell : cells){
        this->_cells.push_back(Cell(*grid_cell, this->_vertices, this->_interfaces));
    }
}

void FluidBlock::compute_fluxes(){
    #ifdef GPU
        #pragma omp target
    #endif
    #pragma omp parallel for
    for (Interface interface : this->_interfaces){
        interface.compute_flux(); 
    }
}

void FluidBlock::compute_time_derivatives(){
    #ifdef GPU
        #pragma omp target
    #endif
    #pragma omp parallel for
    for (Cell cell : this->_cells){
        cell.compute_time_derivative();
    }
}

double FluidBlock::compute_block_dt(double cfl){
    double dt = 10000;
    #ifdef GPU
        #pragma omp target
    #endif
    #pragma omp parallel for reduction(min:dt)
    for (Cell cell : this->_cells){
        dt = std::min(cell.compute_local_timestep(cfl), dt); 
    }
    this->_dt = dt;
    return dt;
}

void FluidBlock::apply_time_derivative(){
    #ifdef GPU
        #pragma omp target
    #endif
    #pragma omp parallel for
    for (Cell cell : this->_cells){
        ConservedQuantity & cq = cell.conserved_quantities;
        for (unsigned int i=0; i < cq.n_conserved(); i++){
            cq[i] += cell.residual[i] * this->_dt; 
        }
        cell.decode_conserved();
    }
}

void FluidBlock::reconstruct(){
    #ifdef GPU
        #pragma omp target
    #endif
    #pragma omp parallel for
    for (Interface face : this->_interfaces){
        face.copy_left_flow_state(face.get_left_cell()->fs);
        face.copy_right_flow_state(face.get_right_cell()->fs);
    }
}

void FluidBlock::fill(std::function<FlowState(double, double, double)> &func){
    for (Cell cell : this->_cells) {
        Vector3 pos = cell.get_pos();
        FlowState fs = func(pos.x, pos.y, pos.z);
        cell.fs.copy(fs);
        cell.encode_conserved();
    }
}

void FluidBlock::fill(const FlowState & fs){
    for (Cell cell : this->_cells) {
        cell.fs.copy(fs);
        cell.encode_conserved();
    }
}


std::vector<Cell> & FluidBlock::cells() {
    return this->_cells;
}

std::vector<Vertex> & FluidBlock::vertices() {
    return this->_vertices;
}

std::string FluidBlock::to_string() {
    std::string str = "FluidBlock(";
    str.append("n_interfaces = ");
    str.append(std::to_string(this->_interfaces.size()));
    str.append(", n_cells = ");
    str.append(std::to_string(this->_cells.size()));
    str.append(", cells = [");
    for (Cell cell : this->_cells){
        str.append(cell.to_string());
        str.append(",\n\n");
    }
    str.append(")");
    return str;
}

std::ostream& operator << (std::ostream& os, const FluidBlock fluid_block){
    os << "FluidBlock(";
    os << "n_interfaces = " << fluid_block._interfaces.size() << ", ";
    for (Cell cell : fluid_block._cells){
        os << cell << ", ";
    }
    os << ")\n";
    return os;
}

void FluidBlock::_print_interfaces(){
    for (Interface interface : this->_interfaces){
        std::cout << interface << "\n";
    }
}
