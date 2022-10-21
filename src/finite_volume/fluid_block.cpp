#include "fluid_block.h"
#include "io/fluid_block_io.h"
#include "config.h"

FluidBlock::~FluidBlock () {}

FluidBlock::FluidBlock(Grid::Grid & grid, unsigned int id, 
                       std::map<std::string, BoundaryCondition> & bc_map) :
    _id(id)
{
    // allocate memory for the vertices, interfaces, and cells
    std::vector<Grid::Vertex *> &vertices = grid.vertices();
    std::vector<Grid::Interface *> &interfaces = grid.interfaces();
    std::vector<Grid::Cell *> &cells = grid.cells();
    this->_vertices = std::vector<Vertex> (vertices.size());
    this->_interfaces = std::vector<Interface> (interfaces.size());

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
        this->_interfaces[id] = Interface(*grid_interface);
        if (grid_interface->is_on_boundary()) n_boundaries += 1;
    }

    this->_number_valid_cells = cells.size();
    this->_number_ghost_cells = n_boundaries;
    int total_cells = this->_number_valid_cells + this->_number_ghost_cells;
    this->_cells = std::vector<Cell> (total_cells);
    // read the cells
    // #pragma omp parallel for
    for (Grid::Cell * grid_cell : cells){
        int id = grid_cell->id();
        this->_cells[id] = Cell(*grid_cell, this->_interfaces);
    }

    // read the boundary conditions
    // this->_ghost_cells.reserve(n_boundaries);
    this->_bcs.reserve(grid.bcs().size());
    int ghost_cell_indx = this->_number_valid_cells;
    for (auto & pair : grid.bcs()){
        std::string bc_key = pair.first;
        std::vector<Grid::Interface *> bc_interfaces = pair.second;
        this->_bcs.push_back(bc_map[bc_key]);
        BoundaryCondition & bc = this->_bcs.back();
        for (Grid::Interface * bc_face : bc_interfaces){
            Interface & fv_interface = this->_interfaces[bc_face->id()];
            fv_interface.mark_on_boundary();
            bc.add_interface(fv_interface);
            this->_cells[ghost_cell_indx] = Cell(
                this->_interfaces[bc_face->id()], ghost_cell_indx, false
            );

            // attach ghost cells to the boundaries
            Cell & ghost_cell = this->_cells[ghost_cell_indx];
            // bc.add_ghost_cell(ghost_cell);
            int left_cell = fv_interface.get_left_cell();
            int right_cell = fv_interface.get_right_cell();
            if (left_cell >= 0 && right_cell >= 0){
                std::string msg = "Internal cell on both sides of boundary\n";
                msg += "left cell = " + this->_cells[left_cell].to_string();
                msg += "\nright cell = " + this->_cells[right_cell].to_string();
                throw std::runtime_error(msg);
            }
            if (left_cell < 0 && right_cell < 0){
                throw std::runtime_error("No valid cell attached to this boundary");
            }
            if (left_cell < 0){
                fv_interface.attach_cell_left(ghost_cell);
            }
            else if (right_cell < 0){
                fv_interface.attach_cell_right(ghost_cell);
            }
            ghost_cell_indx++;
        }
    }
}

void FluidBlock::set_flux_calculator(FluxCalculators flux_calc){
    switch (flux_calc) {
        case FluxCalculators::hanel:
            this->_flux_calculator = &FluxCalculator::hanel;
            break;
        case FluxCalculators::ausmdv:
            this->_flux_calculator = &FluxCalculator::ausmdv;
            break;
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
        interfaces_ptr[i].compute_flux(this->_flux_calculator);
    }
}

void FluidBlock::compute_time_derivatives(){
    int number_cells = this->_number_valid_cells;
    Cell * cell_ptrs = this->_cells.data();

    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for
    for (int i = 0; i < number_cells; i++){
        cell_ptrs[i].compute_time_derivative(this->_interfaces);
    }
}

double FluidBlock::compute_block_dt(double cfl){
    double dt = 10000;
    unsigned int N = this->_number_valid_cells;
    Cell * cell_ptr = this->_cells.data();

    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for reduction(min:dt)
    for (unsigned int i = 0; i < N; i++){
        dt = std::min(cell_ptr[i].compute_local_timestep(cfl, this->_interfaces), dt); 
    }
    this->_dt = dt;
    return dt;
}

void FluidBlock::apply_time_derivative(){
    unsigned int n_cells = this->_number_valid_cells;
    Cell * cell = this->_cells.data();

    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for
    for (unsigned int i_cell = 0; i_cell < n_cells; i_cell++){
        ConservedQuantity & cq = cell[i_cell].conserved_quantities;
        for (unsigned int i_cq=0; i_cq < cq.n_conserved(); i_cq++){
            cq[i_cq] += cell[i_cell].residual[i_cq] * this->_dt; 
        }
        cell[i_cell].decode_conserved(*this->_gas_model);
    }
}

void FluidBlock::reconstruct(){
    //#ifdef GPU
    //    #pragma omp target
    //#endif
    #pragma omp parallel for
    for (Interface & face : this->_interfaces){
        face.copy_left_flow_state(this->_cells[face.get_left_cell()].fs);
        face.copy_right_flow_state(this->_cells[face.get_right_cell()].fs);
    }
}

void FluidBlock::fill(std::function<FlowState(double, double, double)> &func){
    // when calling from python, this can't be done parallel
    for (int i=0; i < this->_number_valid_cells; i++) {
        Cell &cell = this->_cells[i];
        Vector3 pos = cell.get_pos();
        FlowState fs = func(pos.x, pos.y, pos.z);
        cell.fs.copy(fs);
        cell.encode_conserved(*this->_gas_model);
    }
}

void FluidBlock::fill(const FlowState & fs){
    //#ifdef GPU
    //    #pragma omp target
    // #endif
    // #pragma omp parallel for
    for (int i=0; i < this->_number_valid_cells; i++) {
        Cell &cell = this->_cells[i];
        cell.fs.copy(fs);
        cell.encode_conserved(*this->_gas_model);
    }
}


std::vector<Cell> & FluidBlock::cells() {
    return this->_cells;
}

std::vector<Cell> FluidBlock::valid_cells(){
    auto start = this->_cells.begin();
    auto end = this->_cells.begin()+this->_number_valid_cells;
    std::vector<Cell> valid_cells(start, end);
    return valid_cells;
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
