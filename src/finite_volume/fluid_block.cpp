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
    flux_calculator fc = this->_flux_calculator;

    #ifdef GPU
        #pragma omp target teams distribute parallel for simd map(to: interfaces_ptr[:number_interfaces])
    #else
        #pragma omp parallel for 
    #endif
    for (int i = 0; i < number_interfaces; i++){
        interfaces_ptr[i].compute_flux(fc);
    }
}

void FluidBlock::compute_time_derivatives(){
    int number_cells = this->_number_valid_cells;
    int number_interfaces = this->_interfaces.size();
    Cell * cell_ptrs = this->_cells.data();
    Interface * face_ptrs = this->_interfaces.data();

    #ifdef GPU
        #pragma omp target teams distribute parallel for simd //map(to: face_ptrs[:number_interfaces]) map(to: cell_ptrs[:number_cells])
    #else
        #pragma omp parallel for
    #endif
    for (int i = 0; i < number_cells; i++){
        cell_ptrs[i].compute_time_derivative(face_ptrs);
    }
}

double FluidBlock::compute_block_dt(double cfl){
    double dt = 10000;
    unsigned int N = this->_number_valid_cells;
    Cell * cells = this->_cells.data();
    Interface * interfaces = this->_interfaces.data();
    int number_interfaces = this->_interfaces.size();

    #ifdef GPU
        #pragma omp target teams distribute parallel for simd reduction(min:dt) map(tofrom: dt)//map(to: interfaces[0:number_interfaces]) map(to: cells[0:N]) map(tofrom: dt)
    #else
        #pragma omp parallel for reduction(min:dt)
    #endif
    for (unsigned int i = 0; i < N; i++){
        dt = std::min(cells[i].compute_local_timestep(cfl, interfaces), dt); 
    }
    this->_dt = dt;
    return dt;
}

void FluidBlock::apply_time_derivative(){
    int n_cells = this->_number_valid_cells;
    Cell * cell = this->_cells.data();
    double dt = this->_dt;
    GasModel gm = *this->_gas_model;

    #ifdef GPU
        #pragma omp target teams distribute parallel for simd //map(tofrom: cell[0:n_cells])
    #else
        #pragma omp parallel for
    #endif
    for (unsigned int i_cell = 0; i_cell < n_cells; i_cell++){
        ConservedQuantity & cq = cell[i_cell].conserved_quantities;
        for (unsigned int i_cq=0; i_cq < cq.n_conserved(); i_cq++){
            cq[i_cq] += cell[i_cell].residual[i_cq] * dt; 
        }
        cell[i_cell].decode_conserved(gm);
    }
#ifdef GPU
#pragma omp target exit data map(from: cell[0:n_cells])
#endif
}

void FluidBlock::reconstruct(){
    Interface * interfaces = this->_interfaces.data();
    int num_interfaces = this->_interfaces.size();
    Cell * cells = this->_cells.data();
    int num_cells = this->_cells.size();

    #ifdef GPU
        #pragma omp target enter data map(to: interfaces[:num_interfaces]) map(to:cells[:num_cells])
        #pragma omp target teams distribute parallel for simd //map(to: interfaces[:num_interfaces]) map(to: cells[:num_cells])
    #else
        #pragma omp parallel for
    #endif
    for (int i_face = 0; i_face < num_interfaces; i_face++){
        Interface & face = interfaces[i_face];
        face.copy_left_flow_state(cells[face.get_left_cell()].fs);
        face.copy_right_flow_state(cells[face.get_right_cell()].fs);
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
