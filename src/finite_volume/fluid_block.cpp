#include "fluid_block.h"
#include "io/fluid_block_io.h"
#include "config.h"

FluidBlock::~FluidBlock () {}

FluidBlock::FluidBlock(Grid::Grid & grid, unsigned int id, 
                       std::map<std::string, BoundaryCondition> & bc_map) :
    _id(id)
{
    // allocate memory for the vertices, interfaces, and cells
    std::vector<Grid::Vertex *> vertices = grid.vertices();
    std::vector<Grid::Interface *> interfaces = grid.interfaces();
    std::vector<Grid::Cell *> cells = grid.cells();
    this->_vertices = std::vector<Vertex> (vertices.size());
    this->_interfaces = std::vector<Interface> (interfaces.size());
    this->_cells = std::vector<Cell> (cells.size());

    // read the vertices
    // #pragma omp parallel for
    for (Grid::Vertex * grid_vertex : vertices){
        int id = grid_vertex->id();
        this->_vertices[id] = Vertex(*grid_vertex);
    }

    // read the interfaces
    int n_boundaries = 0;
    // # pragma omp parallel for
    for (Grid::Interface * grid_interface : interfaces){
        int id = grid_interface->id();
        this->_interfaces[id] = Interface(*grid_interface, this->_vertices);
        if (grid_interface->is_on_boundary()) n_boundaries += 1;
    }

    // read the cells
    // #pragma omp parallel for
    for (Grid::Cell * grid_cell : cells){
        int id = grid_cell->id();
        this->_cells[id] = Cell(*grid_cell, this->_vertices, this->_interfaces);
    }

    // circle back and attach cells to interfaces
    // there may not be a valid cell on the boundaries, so double
    // check before de-refereincing the pointer
    // we have to attach valid cells first, because attaching ghost
    // cells relies on the valid cells already being attached to know
    // which side they should attach to
    // # pragma omp parallel for
    for (unsigned int i = 0; i < interfaces.size(); i++){
        int left_cell_id = interfaces[i]->get_left_cell_id();
        int right_cell_id = interfaces[i]->get_right_cell_id();
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
    this->_ghost_cells.reserve(n_boundaries);
    this->_bcs.reserve(grid.bcs().size());
    for (auto & pair : grid.bcs()){
        std::string bc_key = pair.first;
        std::vector<Grid::Interface *> bc_interfaces = pair.second;
        this->_bcs.push_back(bc_map[bc_key]);
        BoundaryCondition & bc = this->_bcs.back();
        for (Grid::Interface * bc_face : bc_interfaces){
            Interface & fv_interface = this->_interfaces[bc_face->id()];
            fv_interface.mark_on_boundary(bc_key);
            bc.add_interface(fv_interface);
            this->_ghost_cells.push_back(Cell(this->_interfaces[bc_face->id()], false));

            // attach ghost cells to the boundaries
            Cell & ghost_cell = this->_ghost_cells.back();
            bc.add_ghost_cell(ghost_cell);
            Cell * left_cell = fv_interface.get_left_cell();
            Cell * right_cell = fv_interface.get_right_cell();
            if (left_cell && right_cell){
                std::string msg = "Internal cell on both sides of boundary\n";
                msg += "left cell = " + left_cell->to_string();
                msg += "\nright cell = " + right_cell->to_string();
                throw std::runtime_error(msg);
            }
            if (left_cell){
                fv_interface.attach_cell_right(ghost_cell);
            }
            else if (right_cell){
                fv_interface.attach_cell_left(ghost_cell);
            }
            else {
                throw std::runtime_error("No valid cell attached to boundary interface");
            }
        }
    }
}

void FluidBlock::set_flux_calculator(FluxCalculators flux_calc){
    #pragma omp parallel for
    for (Interface & interface : this->_interfaces){
        interface.set_flux_calculator(flux_calc);
    }
}

void FluidBlock::compute_fluxes(){
    int number_interfaces = this->_interfaces.size();
    Interface * interfaces_ptr = this->_interfaces.data();

    #ifdef GPU
        #pragma omp target
    #endif
    #pragma omp parallel for
    for (int i = 0; i < number_interfaces; i++){
        interfaces_ptr[i].compute_flux();
    }
}

void FluidBlock::compute_time_derivatives(){
    int number_cells = this->_cells.size();
    Cell * cell_ptrs = this->_cells.data();

    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for
    for (int i = 0; i < number_cells; i++){
        cell_ptrs[i].compute_time_derivative();
    }
}

double FluidBlock::compute_block_dt(double cfl){
    double dt = 10000;
    unsigned int N = this->_cells.size();
    Cell * cell_ptr = this->_cells.data();

    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for reduction(min:dt)
    for (unsigned int i = 0; i < N; i++){
        dt = std::min(cell_ptr[i].compute_local_timestep(cfl), dt); 
    }
    this->_dt = dt;
    return dt;
}

void FluidBlock::apply_time_derivative(){
    unsigned int n_cells = this->_cells.size();
    Cell * cell = this->_cells.data();

    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for
    for (unsigned int i = 0; i < n_cells; i++){
        ConservedQuantity & cq = cell[i].conserved_quantities;
        for (unsigned int i=0; i < cq.n_conserved(); i++){
            cq[i] += cell[i].residual[i] * this->_dt; 
        }
        cell[i].decode_conserved(*this->_gas_model);
    }
}

void FluidBlock::reconstruct(){
    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for
    for (Interface & face : this->_interfaces){
        face.copy_left_flow_state(face.get_left_cell()->fs);
        face.copy_right_flow_state(face.get_right_cell()->fs);
    }
}

void FluidBlock::fill(std::function<FlowState(double, double, double)> &func){
    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for
    for (Cell & cell : this->_cells) {
        Vector3 pos = cell.get_pos();
        FlowState fs = func(pos.x, pos.y, pos.z);
        cell.fs.copy(fs);
        cell.encode_conserved(*this->_gas_model);
    }
}

void FluidBlock::fill(const FlowState & fs){
    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for
    for (Cell & cell : this->_cells) {
        cell.fs.copy(fs);
        cell.encode_conserved(*this->_gas_model);
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
