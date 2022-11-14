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
    FlowState fs;
    fs.gas_state = gs;
    fs.velocity = vel;
    return fs;
}

#ifdef GPU
#pragma omp declare target
#endif
void FluidBlock::set_flow(int i, FlowState flow){
    this->_p[i] = flow.gas_state.p;
    this->_T[i] = flow.gas_state.T;
    this->_rho[i] = flow.gas_state.rho;
    this->_u[i] = flow.gas_state.u;
    this->_a[i] = flow.gas_state.a;
    this->_vx[i] = flow.velocity.x;
    this->_vy[i] = flow.velocity.y;
    this->_vz[i] = flow.velocity.z;
}
#ifdef GPU
#pragma omp end declare target
#endif

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

#define GET_CELL_PTRS \
    int num_cells = this->_cells.size();\
    int num_valid_cells = this->_number_valid_cells;\
    Cell *cells = this->_cells.data();

#define GET_INTERFACE_PTRS\
    int num_faces = this->_interfaces.size();\
    Interface *faces = this->_interfaces.data();

#define GET_FLOW_PTRS \
    double *p = this->_p.data(); \
    double *T = this->_T.data(); \
    double *rho = this->_rho.data(); \
    double *u = this->_u.data(); \
    double *a = this->_a.data(); \
    double *vx = this->_vx.data(); \
    double *vy = this->_vy.data(); \
    double *vz = this->_vz.data();

#define GET_CONSERVED_PTRS \
    double *mass = this->_mass.data();\
    double *px = this->_px.data();\
    double *py = this->_py.data();\
    double *pz = this->_pz.data();\
    double *e = this->_e.data();

#define GET_RESIDUAL_PTRS \
    double *dmass_dt = this->_dmass_dt.data();\
    double *dpx_dt = this->_dpx_dt.data();\
    double *dpy_dt = this->_dpy_dt.data();\
    double *dpz_dt = this->_dpz_dt.data();\
    double *de_dt = this->_de_dt.data();

void FluidBlock::reconstruct(){
    GET_INTERFACE_PTRS
    GET_FLOW_PTRS
    #ifdef GPU
        GET_CELL_PTRS
        GET_CONSERVED_PTRS
        GET_RESIDUAL_PTRS
        #pragma omp target enter data\
            map(to: p[:num_cells])\
            map(to: T[:num_cells])\
            map(to: rho[:num_cells])\
            map(to: u[:num_cells])\
            map(to: a[:num_cells])\
            map(to: vx[:num_cells])\
            map(to: vy[:num_cells])\
            map(to: vz[:num_cells])\
            map(to: mass[:num_cells])\
            map(to: px[:num_cells])\
            map(to: py[:num_cells])\
            map(to: pz[:num_cells])\
            map(to: e[:num_cells])\
            map(to: dmass_dt[:num_cells])\
            map(to: dpx_dt[:num_cells])\
            map(to: dpy_dt[:num_cells])\
            map(to: dpz_dt[:num_cells])\
            map(to: de_dt[:num_cells])\
            map(to: faces[:num_faces])\
            map(to: cells[:num_cells])\

        #pragma omp target teams distribute parallel for simd
    #else
        #pragma omp parallel for
    #endif
    for (int i_face = 0; i_face < num_faces; i_face++){
        Interface &face = faces[i_face];
        int id_left = face.get_left_cell();
        int id_right = face.get_right_cell();

        // left flow state
        double p_left = p[id_left];
        double T_left = T[id_left];
        double rho_left = rho[id_left];
        double a_left = a[id_left];
        double u_left = u[id_left];
        double vx_left = vx[id_left];
        double vy_left = vy[id_left];
        double vz_left = vz[id_left];

        // right flow state
        double p_right = p[id_right];
        double T_right = T[id_right];
        double rho_right = rho[id_right];
        double a_right = a[id_right];
        double u_right = u[id_right];
        double vx_right = vx[id_right];
        double vy_right = vy[id_right];
        double vz_right = vz[id_right];
        
        // copy data
        face.copy_left_flow_state(p_left, T_left, rho_left, u_left, a_left,
                                  vx_left, vy_left, vz_left);
        face.copy_right_flow_state(p_right, T_right, rho_right, u_right, a_right,
                                  vx_right, vy_right, vz_right);
    }
}

void FluidBlock::compute_fluxes(){
    GET_INTERFACE_PTRS
    flux_calculator fc = this->_flux_calculator;

    #ifdef GPU
        #pragma omp target teams distribute parallel for simd
    #else
        #pragma omp parallel for 
    #endif
    for (int i = 0; i < num_faces; i++){
        faces[i].compute_flux(fc);
    }
}

void FluidBlock::compute_residuals(){
    GET_RESIDUAL_PTRS
    GET_INTERFACE_PTRS
    GET_CELL_PTRS

    #ifdef GPU
        #pragma omp target teams distribute parallel for
    #else
        #pragma omp parallel for
    #endif
    for (int i = 0; i < num_valid_cells; i++){
        Cell &cell = cells[i];
        double mass_si = 0.0;
        double px_si = 0.0;
        double py_si = 0.0;
        double pz_si = 0.0;
        double e_si = 0.0;
        for (int i_face=0; i_face < cell.number_interfaces(); ++i_face){
            const CellFace &face = cell.interfaces()[i_face];
            Interface & f = faces[face.interface];
            double area = (face.outwards ? -1 : 1) * f.area(); 

            mass_si += area * f.flux().mass;
            px_si += area * f.flux().momentum.x;
            py_si += area * f.flux().momentum.y;
            pz_si += area * f.flux().momentum.z;
            e_si += area * f.flux().energy;
        }
        dmass_dt[i] = mass_si / cell.volume();
        dpx_dt[i] = px_si / cell.volume();
        dpy_dt[i] = py_si / cell.volume();
        dpz_dt[i] = pz_si / cell.volume();
        de_dt[i] = e_si / cell.volume();
    }
}

double FluidBlock::compute_block_dt(double cfl){
    double dt = 10000;
    GET_CELL_PTRS
    GET_FLOW_PTRS
    GET_INTERFACE_PTRS

    #ifdef GPU
        #pragma omp target teams distribute parallel for reduction(min:dt) map(tofrom: dt)
    #else
        #pragma omp parallel for reduction(min:dt)
    #endif
    for (unsigned int i_cell = 0; i_cell < num_valid_cells; i_cell++){
        // compute local dt for a particular cell
        double spectral_radii = 0.0;
        Cell &cell = cells[i_cell]; 
        for (int i_face = 0; i_face < cell.number_interfaces(); i_face++){
            const CellFace &face = cell.interfaces()[i_face];
            Interface & f = faces[face.interface];
            double dot = vx[i_cell]*f.n().x + vy[i_cell]*f.n().y + vz[i_cell]*f.n().z;
            double sig_vel = fabs(dot) + a[i_cell];
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
    int num_valid_cells = this->_number_valid_cells;
    GET_RESIDUAL_PTRS
    GET_CONSERVED_PTRS
    #ifdef GPU
        #pragma omp target teams distribute parallel for simd
    #else
        #pragma omp parallel for
    #endif
    for (int i_cell = 0; i_cell < num_valid_cells; i_cell++){
        mass[i_cell] += dmass_dt[i_cell] * dt;
        px[i_cell] += dpx_dt[i_cell] * dt;
        py[i_cell] += dpy_dt[i_cell] * dt;
        pz[i_cell] += dpz_dt[i_cell] * dt;
        e[i_cell] += de_dt[i_cell] * dt;
    }
    this->decode_conserved(true);

    #ifdef GPU
        GET_FLOW_PTRS
        #pragma omp target exit data\
            map(from: p[:num_valid_cells])\
            map(from: T[:num_valid_cells])\
            map(from: rho[:num_valid_cells])\
            map(from: u[:num_valid_cells])\
            map(from: a[:num_valid_cells])\
            map(from: vx[:num_valid_cells])\
            map(from: vy[:num_valid_cells])\
            map(from: vz[:num_valid_cells])
    #endif
}

void FluidBlock::encode_conserved(bool gpu){
    int number_valid_cells = this->_number_valid_cells;
    GET_CONSERVED_PTRS
    GET_FLOW_PTRS

    #ifdef GPU
        #pragma omp target teams distribute parallel for if (gpu)
    #else
        #pragma omp parallel for
    #endif
    for (int i=0; i < number_valid_cells; ++i){
        double rhoi = rho[i];
        double pi = p[i];
        double ui = u[i];
        double vxi = vx[i];
        double vyi = vy[i];
        double vzi = vz[i];
        double kei = 0.5*(vxi*vxi + vyi*vyi + vzi*vzi);
        mass[i] = rhoi;
        px[i] = rhoi * vxi;
        py[i] = rhoi * vyi;
        pz[i] = rhoi * vzi;
        e[i] = (ui + kei)*rhoi + pi;
    }
}

void FluidBlock::decode_conserved(bool gpu){
    GET_FLOW_PTRS
    GET_CONSERVED_PTRS
    GasModel gm = *this->_gas_model;
    int number_cells = this->_number_valid_cells;
    #ifdef GPU
        #pragma omp target teams distribute parallel for map(to: gm) if(gpu)
    #else
        #pragma omp parallel for
    #endif
    for (int i=0; i < number_cells; ++i){
        double vxi = vx[i];
        double vyi = vy[i];
        double vzi = vz[i];
        double kei = 0.5*(vxi*vxi + vyi*vyi + vzi*vzi);
        double rhoi = mass[i];
        double pi = p[i];
        vx[i] = px[i] / rhoi;
        vy[i] = py[i] / rhoi;
        vz[i] = pz[i] / rhoi;
        rho[i] = mass[i];
        u[i] = (e[i] - pi)/rhoi - kei;
        double Cv = gm.Cv();
        double R = gm.R();
        double gamma = gm.gamma();
        T[i] = u[i] / Cv;
        p[i] = rho[i] * R * T[i];
        a[i] = sqrt(gamma * R * T[i]);
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
