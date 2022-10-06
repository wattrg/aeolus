#include "cell.h"

Cell::Cell(Interface * face, bool valid) 
{
    _valid_cell = valid;
    this->_interfaces = std::vector<CellFace> (1);
    this->_interfaces.push_back(CellFace(*face, false)); 
}

Cell::Cell(Grid::Cell & grid_cell, std::vector<Vertex> & vertices, std::vector<Interface> & interfaces)
    : _shape(grid_cell.get_shape()),
      _volume(grid_cell.volume()),
      _pos(grid_cell.get_pos())
{
    std::vector<Grid::CellFace> grid_interfaces = grid_cell.interfaces();
    std::vector<Grid::Vertex *> grid_vertices = grid_cell.vertices();

    // allocate memory for pointers to interfaces and vertices
    size_t num_interfaces = grid_interfaces.size();
    size_t num_vertices = grid_vertices.size();
    //this->_interfaces = std::vector<CellFace>(num_interfaces);
    //this->_vertices = std::vector<Vertex *>(num_vertices);

    // assign pointers to interfaces and vertices
    for (size_t i = 0; i < num_vertices; i++){
        size_t id = grid_vertices[i]->id();
        this->_vertices[i] = &vertices[id];
    }
    for (size_t i = 0; i < num_interfaces; i++){
        Grid::CellFace grid_interface = grid_interfaces[i];
        size_t id = grid_interface.interface->id();
        this->_interfaces[i] = CellFace(interfaces[id], grid_interface.outwards);
    }

    // initialise conserved quantities and residuals
    this->conserved_quantities = ConservedQuantity(2);
    this->residual = ConservedQuantity(2);

}


double Cell::compute_local_timestep(double cfl){
    double spectral_radii = 0.0;
    for (CellFace face : this->_interfaces){
        double sig_vel = fabs(this->fs.velocity.dot(face.interface->n())) + this->fs.gas_state.a;
        spectral_radii += sig_vel * face.interface->area();
    }
    double dt = cfl * this->_volume / spectral_radii;
    if (_lts) this->_dt = dt;
    return dt;
}

double Cell::volume() const {return this->_volume;}

void Cell::compute_time_derivative(){
    int n_conserved = this->conserved_quantities.n_conserved();
    for (int i = 0; i < n_conserved; i++){
        double surface_integral = 0.0;
        for (CellFace face : this->_interfaces){
            Interface * fvface = face.interface;
            double area = (face.outwards ? -1 : 1) * fvface->area(); 
            surface_integral += area * fvface->flux()[i];
        }
        this->residual[i] = surface_integral / this->_volume;
    }
}

void Cell::encode_conserved(){
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
    cq[cq.energy()] = u + ke + p/rho;
}

void Cell::decode_conserved(){
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
    this->_gas_model->update_from_rhou(fs.gas_state);
}

Vector3 & Cell::get_pos(){
    return this->_pos;
}

unsigned int Cell::number_vertices() const {
    return this->_vertices.size();
}

const std::vector<Vertex *> & Cell::vertices() const {
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
    for (Vertex * vertex : this->_vertices){
        str.append(vertex->to_string());
    }
    str.append("])");
    return str;
}

std::ostream& operator << (std::ostream& os, const Cell cell){
    os << cell.to_string();
    return os;
}
