#include "cell.h"
#include "../util/unused.h"

Cell::Cell(Interface & face, int id, bool valid) 
    : _valid_cell(valid),
      _id(id)
{
    this->_interfaces[0] = CellFace(face, false); 
}

Cell::Cell(Grid::Cell & grid_cell, std::vector<Interface> & interfaces)
    : _pos(grid_cell.get_pos()), 
      _shape(grid_cell.get_shape()),
      _valid_cell(true),
      _volume(grid_cell.volume()),
      _id(grid_cell.id())
{
    std::vector<Grid::CellFace> grid_interfaces = grid_cell.interfaces();
    std::vector<Grid::Vertex *> grid_vertices = grid_cell.vertices();

    this->_number_interfaces = grid_interfaces.size();
    this->_number_vertices = grid_vertices.size();

    // assign pointers to interfaces and vertices
    for (int i = 0; i < this->_number_vertices; i++){
        size_t id = grid_vertices[i]->id();
        this->_vertices[i] = id;
    }

    for (int i = 0; i < this->_number_interfaces; i++){
        Grid::CellFace grid_interface = grid_interfaces[i];
        size_t id = grid_interface.interface->id();
        this->_interfaces[i] = CellFace(interfaces[id], grid_interface.outwards);
    }

    // initialise conserved quantities and residuals
    this->conserved_quantities = ConservedQuantity(2);
    this->residual = ConservedQuantity(2);
}


#ifdef GPU
#pragma omp declare target
#endif
double Cell::compute_local_timestep(double cfl, Interface * faces){

    double spectral_radii = 0.0;
    for (int i = 0; i < this->_number_interfaces; i++){
        CellFace &face = this->_interfaces[i];
        Interface & f = faces[face.interface];
        double sig_vel = fabs(this->fs.velocity.dot(f.n())) + this->fs.gas_state.a;
        spectral_radii += sig_vel * f.area();
    }
    double dt = cfl * this->_volume / spectral_radii;
    if (_lts) this->_dt = dt;
    return dt;
}
#ifdef GPU
#pragma omp end declare target
#endif

double Cell::volume() const {return this->_volume;}

#ifdef GPU
#pragma omp declare target
#endif
void Cell::compute_time_derivative(Interface * faces){
    int n_conserved = this->conserved_quantities.n_conserved();
    for (int i_cq = 0; i_cq < n_conserved; i_cq++){
        double surface_integral = 0.0;
        for (int i_face=0; i_face < this->_number_interfaces; i_face++){
            CellFace &face = this->_interfaces[i_face];
            Interface & f = faces[face.interface];
            double area = (face.outwards ? -1 : 1) * f.area(); 
            surface_integral += area * f.flux()[i_cq];
        }
        this->residual[i_cq] = surface_integral / this->_volume;
    }
}
#ifdef GPU
#pragma omp end declare target
#endif

void Cell::encode_conserved(GasModel & gas_model){
    UNUSED(gas_model);
    double vx = this->fs.velocity.x;
    double vy = this->fs.velocity.y;
    double vz = this->fs.velocity.z;
    double p = this->fs.gas_state.p;
    double rho = this->fs.gas_state.rho;
    double u = this->fs.gas_state.u;
    double ke = 0.5 * (vx*vx + vy*vy + vz*vz);
    ConservedQuantity & cq = this->conserved_quantities;
    cq[cq.rho()] = rho;
    cq[cq.momentum()] = rho*vx;
    cq[cq.momentum()+1] = rho*vy;
    cq[cq.energy()] = (u + ke)*rho + p;
}

#ifdef GPU
#pragma omp declare target
#endif
void Cell::decode_conserved(GasModel & gas_model){
    double vx = this->fs.velocity.x;
    double vy = this->fs.velocity.y;
    double vz = this->fs.velocity.z;
    double ke = 0.5 * (vx*vx + vy*vy + vz*vz);
    ConservedQuantity & cq = this->conserved_quantities;
    this->fs.gas_state.rho = cq[cq.rho()];
    this->fs.velocity.x = cq[cq.momentum()] / cq[cq.rho()];
    this->fs.velocity.y = cq[cq.momentum()+1] / cq[cq.rho()];
    double e = cq[cq.energy()]/fs.gas_state.rho;
    this->fs.gas_state.u = e - ke;
    gas_model.update_from_rhou(fs.gas_state);
}
#ifdef GPU
#pragma omp end declare target
#endif

Vector3 & Cell::get_pos(){
    return this->_pos;
}

unsigned int Cell::number_vertices() const {
    return this->_number_vertices;
}

int * Cell::vertices() {
    return this->_vertices;
}

Grid::CellShape Cell::get_shape() const {
    return _shape; 
}

std::string Cell::to_string() const {
    std::string str = "Cell(";
    str.append(this->_pos.to_string());
    str.append(this->fs.to_string());
    str.append(", ");
    str.append("vertices = [");
    for (int i = 0; i < this->_number_vertices; i++){
        str.append(std::to_string(i));
    }
    str.append("])");
    return str;
}

std::ostream& operator << (std::ostream& os, const Cell cell){
    os << cell.to_string();
    return os;
}
