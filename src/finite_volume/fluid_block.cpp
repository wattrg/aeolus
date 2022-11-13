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

    // allocate memory
    this->_number_valid_cells = cells.size();
    this->_number_ghost_cells = n_boundaries;
    int total_cells = this->_number_valid_cells + this->_number_ghost_cells;

    this->_cells = std::vector<Cell> (total_cells);
    this->_p = std::vector<double> (total_cells);
    this->_T = std::vector<double> (total_cells);
    this->_u = std::vector<double> (total_cells);
    this->_rho = std::vector<double> (total_cells);
    this->_a = std::vector<double> (total_cells);
    this->_vx = std::vector<double> (total_cells);
    this->_vy = std::vector<double> (total_cells);
    this->_vz = std::vector<double> (total_cells);

    this->_mass = std::vector<double> (total_cells);
    this->_px = std::vector<double> (total_cells);
    this->_py = std::vector<double> (total_cells);
    this->_pz = std::vector<double> (total_cells);
    this->_e = std::vector<double> (total_cells);

    this->_dmass_dt = std::vector<double> (total_cells);
    this->_dpx_dt = std::vector<double> (total_cells);
    this->_dpy_dt = std::vector<double> (total_cells);
    this->_dpz_dt = std::vector<double> (total_cells);
    this->_de_dt = std::vector<double> (total_cells);

    // read the cells
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

FlowState FluidBlock::get_flow(int i){
    double p = this->_p[i];
    double T = this->_T[i];
    double rho = this->_rho[i];
    double u = this->_u[i];
    double a = this->_a[i];
    GasState gs;
    gs.p = p;
    gs.T = T;
    gs.rho = rho;
    gs.a = a;
    gs.u = u;
    Vector3 vel = Vector3(this->_vx[i], this->_vy[i], this->_vz[i]);
    return FlowState(gs, vel);
}

void FluidBlock::set_flow(int i, FlowState flow){
    this->_p[i] = flow.gas_state.p;
    this->_T[i] = flow.gas_state.T;
    this->_rho[i] = flow.gas_state.rho;
    this->_u[i] = flow.gas_state.u;
    this->_vx[i] = flow.velocity.x;
    this->_vy[i] = flow.velocity.y;
    this->_vz[i] = flow.velocity.z;
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

void FluidBlock::reconstruct(){
    Interface * interfaces = this->_interfaces.data();
    int num_interfaces = this->_interfaces.size();

    #ifdef GPU
        int num_cells = this->_cells.size();
        #pragma omp target enter data map(to: interfaces[:num_interfaces]) map(to:cells[:num_cells])
        #pragma omp target teams distribute parallel for simd
    #else
        #pragma omp parallel for
    #endif
    for (int i_face = 0; i_face < num_interfaces; i_face++){
        Interface & face = interfaces[i_face];
        int id_left = face.get_left_cell();
        int id_right = face.get_right_cell();
        FlowState left(this->get_flow(id_left));
        FlowState right(this->get_flow(id_right));
        face.copy_left_flow_state(left);
        face.copy_right_flow_state(right);
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

void FluidBlock::compute_residuals(){
    int number_cells = this->_number_valid_cells;
    Cell * cell_ptrs = this->_cells.data();
    Interface * face_ptrs = this->_interfaces.data();

    #ifdef GPU
        #pragma omp target teams distribute parallel for
    #else
        #pragma omp parallel for
    #endif
    for (int i = 0; i < number_cells; i++){
        Cell &cell = cell_ptrs[i];
        double mass_si = 0.0;
        double px_si = 0.0;
        double py_si = 0.0;
        double pz_si = 0.0;
        double e_si = 0.0;
        for (int i_face=0; i_face < cell.number_interfaces(); ++i_face){
            const CellFace &face = cell.interfaces()[i_face];
            Interface & f = face_ptrs[face.interface];
            double area = (face.outwards ? -1 : 1) * f.area(); 

            mass_si += area * f.flux().mass;
            px_si += area * f.flux().momentum.x;
            py_si += area * f.flux().momentum.y;
            pz_si += area * f.flux().momentum.z;
            e_si += area * f.flux().energy;
        }
        this->_dmass_dt[i] = mass_si / cell.volume();
        this->_dpx_dt[i] = px_si / cell.volume();
        this->_dpy_dt[i] = py_si / cell.volume();
        this->_dpz_dt[i] = pz_si / cell.volume();
    }
}

double FluidBlock::compute_block_dt(double cfl){
    double dt = 10000;
    unsigned int N = this->_number_valid_cells;
    Cell * cells = this->_cells.data();
    Interface * interfaces = this->_interfaces.data();

    #ifdef GPU
        #pragma omp target teams distribute parallel for reduction(min:dt) map(tofrom: dt)
    #else
        #pragma omp parallel for reduction(min:dt)
    #endif
    for (unsigned int i_cell = 0; i_cell < N; i_cell++){
        // compute local dt for a particular cell
        double spectral_radii = 0.0;
        Cell &cell = cells[i_cell]; 
        for (int i_face = 0; i_face < cell.number_interfaces(); i_face++){
            const CellFace &face = cell.interfaces()[i_face];
            Interface & f = interfaces[face.interface];
            Vector3 vel = Vector3(_vx[i_cell], _vy[i_cell], _vz[i_cell]);
            double sig_vel = fabs(vel.dot(f.n())) + this->_a[i_cell];
            spectral_radii += sig_vel * f.area();
        }
        double local_dt = cfl * cell.volume() / spectral_radii;

        // reduce to minimum dt
        dt = std::min(local_dt, dt); 
    }
    this->_dt = dt;
    return dt;
}

void FluidBlock::apply_residuals(double dt){
    int n_cells = this->_number_valid_cells;

    #ifdef GPU
        #pragma omp target teams distribute parallel for simd
    #else
        #pragma omp parallel for
    #endif
    for (int i_cell = 0; i_cell < n_cells; i_cell++){
        this->_mass[i_cell] += this->_dmass_dt[i_cell] * dt;
        this->_px[i_cell] += this->_dpx_dt[i_cell] * dt;
        this->_py[i_cell] += this->_dpy_dt[i_cell] * dt;
        this->_pz[i_cell] += this->_dpz_dt[i_cell] * dt;
        this->_e[i_cell] += this->_de_dt[i_cell] * dt;
    }
    this->decode_conserved();

#ifdef GPU
#pragma omp target exit data map(from: cell[0:n_cells])
#endif
}

void FluidBlock::encode_conserved(){
    #ifdef GPU
        #pragma omp target teams distribute parallel for simd
    #else
        #pragma omp parallel for
    #endif
    for (int i=0; i < this->_number_valid_cells; ++i){
        double rho = this->_rho[i];
        double p = this->_p[i];
        double u = this->_u[i];
        double vx = this->_vx[i];
        double vy = this->_vy[i];
        double vz = this->_vz[i];
        double ke = 0.5*(vx*vx + vy*vy + vz*vz);
        this->_mass[i] = rho;
        this->_px[i] = rho * vx;
        this->_py[i] = rho * vy;
        this->_pz[i] = rho * vz;
        this->_e[i] = (u + ke)*rho + p;
    }
}

void FluidBlock::decode_conserved(){
    #ifdef GPU
        #pragma omp target teams distribute parallel for simd
    #else
        #pragma omp parallel for
    #endif
    for (int i=0; i < this->_number_valid_cells; ++i){
        double vx = this->_vx[i];
        double vy = this->_vy[i];
        double vz = this->_vz[i];
        double ke = 0.5*(vx*vx + vy*vy + vz*vz);
        double rho = this->_mass[i];
        this->_vx[i] = this->_px[i] / rho;
        this->_vy[i] = this->_py[i] / rho;
        this->_vz[i] = this->_pz[i] / rho;
        this->_rho[i] = this->_mass[i];
        this->_u[i] = this->_e[i] / rho;
        GasState gs;
        gs.rho = this->_rho[i];
        gs.u = this->_u[i] - ke;
        this->_gas_model->update_from_rhou(gs);
        this->_p[i] = gs.p;
        this->_T[i] = gs.T;
        this->_a[i] = gs.a;
    }
}

void FluidBlock::apply_bcs(){
    for (BoundaryCondition & bc : this->bcs()){
        bc.apply_pre_reconstruction(*this, this->interfaces());
    }
}

void FluidBlock::fill(std::function<FlowState(double, double, double)> &func){
    // when calling from python, this can't be done parallel
    for (int i=0; i < this->_number_valid_cells; i++) {
        Cell &cell = this->_cells[i];
        Vector3 pos = cell.get_pos();
        FlowState fs = func(pos.x, pos.y, pos.z);
        this->set_flow(i, fs);
    }
    this->encode_conserved();
}

void FluidBlock::fill(const FlowState & fs){
    for (int i=0; i < this->_number_valid_cells; i++) {
        this->set_flow(i, fs);
    }
    this->encode_conserved();
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
