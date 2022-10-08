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
    this->_vertices.reserve(vertices.size());
    for (Grid::Vertex * grid_vertex : vertices){
        this->_vertices.push_back(Vertex(*grid_vertex));
    }

    // read the interfaces
    std::vector<Grid::Interface *> interfaces = grid.interfaces();
    this->_interfaces.reserve(interfaces.size());
    for (Grid::Interface * grid_interface : interfaces){
        this->_interfaces.push_back(Interface(*grid_interface, this->_vertices));
    }

    // read the cells
    std::vector<Grid::Cell *> cells = grid.cells();
    this->_cells.reserve(cells.size());
    for (Grid::Cell * grid_cell : cells){
        this->_cells.push_back(Cell(*grid_cell, this->_vertices, this->_interfaces));
    }

    // circle back and attach cells to interfaces
    for (unsigned int i = 0; i < interfaces.size(); i++){
        int left_cell_id = interfaces[i]->get_left_cell_id();
        int right_cell_id = interfaces[i]->get_right_cell_id();
        // there may not be a valid cell on the boundaries
        // we have to attach valid cells first, because attaching ghost
        // cells relies on the valid cells already being attached to know
        // which side they should attach to
        if (left_cell_id >= 0){
            Cell & left_cell = this->_cells[left_cell_id];
            this->_interfaces[i].attach_cell_left(left_cell);
        }
        if (right_cell_id >= 0){
            Cell & right_cell = this->_cells[right_cell_id];
            this->_interfaces[i].attach_cell_right(right_cell);
        }
    }
    

    // read the boundary conditions
    for (auto & pair : grid.bcs()){
        std::string bc_key = pair.first;
        std::vector<Grid::Interface *> bc_interfaces = pair.second;
        for (Grid::Interface * bc_face : bc_interfaces){
            BoundaryCondition & bc = bc_map[bc_key];
            Interface & fv_interface = this->_interfaces[bc_face->id()];
            bc.add_interface(fv_interface);
            this->_ghost_cells.push_back(Cell(this->_interfaces[bc_face->id()]));

            // attach the ghost cell to the interface
            Cell & ghost_cell = this->_ghost_cells[this->_ghost_cells.size()-1];
            bc.add_ghost_cell(ghost_cell);
            Cell & valid_cell = fv_interface.get_valid_cell();
            if (fv_interface.get_left_cell()){
                fv_interface.attach_cell_right(ghost_cell);
            }
            else if (fv_interface.get_right_cell()){
                fv_interface.attach_cell_right(ghost_cell);
            }
            else {
                throw std::runtime_error("No valid cell attached to boundary interface");
            }
        }
    }
}

void FluidBlock::compute_fluxes(){
    #ifdef GPU
        #pragma omp target
    #endif
    #pragma omp parallel for
    for (Interface & interface : this->_interfaces){
        interface.compute_flux(); 
    }
}

void FluidBlock::compute_time_derivatives(){
    #ifdef GPU
        #pragma omp target
    #endif
    #pragma omp parallel for
    for (Cell & cell : this->_cells){
        cell.compute_time_derivative();
    }
}

double FluidBlock::compute_block_dt(double cfl){
    double dt = 10000;
    //#ifdef GPU
    //    #pragma omp target
    //#endif
    //#pragma omp parallel for reduction(min:dt)
    for (Cell & cell : this->_cells){
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
    for (Cell & cell : this->_cells){
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
    for (Interface & face : this->_interfaces){
        face.copy_left_flow_state(face.get_left_cell()->fs);
        face.copy_right_flow_state(face.get_right_cell()->fs);
    }
}

void FluidBlock::fill(std::function<FlowState(double, double, double)> &func){
    for (Cell & cell : this->_cells) {
        Vector3 pos = cell.get_pos();
        FlowState fs = func(pos.x, pos.y, pos.z);
        cell.fs.copy(fs);
        cell.encode_conserved();
    }
}

void FluidBlock::fill(const FlowState & fs){
    for (Cell & cell : this->_cells) {
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
    for (const Cell & cell : fluid_block._cells){
        os << cell << ", ";
    }
    os << ")\n";
    return os;
}

void FluidBlock::_print_interfaces(){
    for (Interface & interface : this->_interfaces){
        std::cout << interface << "\n";
    }
}
