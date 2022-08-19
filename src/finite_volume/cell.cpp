#include "cell.h"

Cell::Cell(Interface * face, Simulation & config, bool valid) : _valid_cell(valid), _config(config) {
    this->_interfaces.push_back(CellFace(*face, false)); 
}

Cell::Cell(std::vector<Vertex*> vertices, 
           std::vector<Interface*> interfaces, 
           Simulation & config,
           unsigned int id, 
           bool valid)
 : _vertices(vertices), _valid_cell(valid), _config(config), _id(id)
{
    // TODO do we need to have vertices passed to the constructor?
    // we could get away with just the interfaces

    //  this->_interfaces = interfaces;
    //

    // find the centroid of the cell
    double centre_x = 0.0;
    double centre_y = 0.0;
    int num_vertices = vertices.size();
    for (Vertex * vertex : _vertices){
        centre_x += vertex->get_pos().x;
        centre_y += vertex->get_pos().y;
    }
    centre_x /= num_vertices;
    centre_y /= num_vertices;
    _pos = Vector3(centre_x, centre_y);

    this->_compute_shape();
    this->_compute_volume();

    //  attach the cell to the interfaces
    bool outward_interface;
    for (Interface * interface : interfaces) {
        outward_interface = interface->attach_cell(*this);
        this->_interfaces.push_back(CellFace(*interface, outward_interface)); 
    }
    
    // initialise conserved quantities
    this->conserved_quantities = ConservedQuantity(2);
    this->residual = ConservedQuantity(2);
}

double Cell::compute_local_timestep(){
    double spectral_radii = 0.0;
    for (CellFace face : this->_interfaces){
        double sig_vel = fabs(this->fs.velocity.dot(face.interface->n())) + this->fs.gas_state.a;
        spectral_radii += sig_vel * face.interface->area();
    }
    double dt = this->_volume / spectral_radii;
    if (_lts) this->_dt = dt;
    return dt;
}

void Cell::_compute_shape(){
    switch (this->_vertices.size()){
        case 3:
            this->_shape = CellShape::Triangle;
            break;
        case 4:
            this->_shape = CellShape::Quad;
            break;
        default:
            throw std::runtime_error("unknown cell shape");
    }
}

const double Cell::volume() const {return this->_volume;}

void Cell::_compute_volume(){
    switch (this->_shape){
        case CellShape::Triangle:
            {
            std::vector<Vector3> points;
            points.reserve(3);
            for (Vertex * vertex : this->_vertices){
                points.push_back(vertex->get_pos());
            }
            this->_volume = triangle_area(points);
            break;
            }
        case CellShape::Quad:
            {
            std::vector<Vector3> points;
            points.reserve(4);
            for (Vertex * vertex : this->_vertices){
                points.push_back(vertex->get_pos());
            }
            this->_volume = quad_area(points);
            break;
            }
        default:
            throw std::runtime_error("unknown cell shape in volume calculation");
    }
}

void Cell::compute_time_derivative(){
    int n_conserved = this->conserved_quantities.n_conserved();
    for (int i = 0; i < n_conserved; i++){
        double surface_integral = 0.0;
        for (CellFace face : this->_interfaces){
            Interface * fvface = face.interface;
            double area = (face.outwards ? 1 : -1) * fvface->area(); 
            surface_integral += area * fvface->flux()[i];
        }
        this->residual[i] = surface_integral / this->_volume;
    }
}

void Cell::encode_conserved(){
    ConservedQuantity & cq = this->conserved_quantities;
    cq[cq.rho()] = this->fs.gas_state.rho;
    cq[cq.momentum()] = this->fs.gas_state.rho * this->fs.velocity.x;
    cq[cq.momentum()+1] = this->fs.gas_state.rho * this->fs.velocity.y;
    double ke = 0.5 * (this->fs.velocity.x*this->fs.velocity.x + this->fs.velocity.y*this->fs.velocity.y);
    cq[cq.energy()] = this->fs.gas_state.u + ke;
}

void Cell::decode_conserved(){
    ConservedQuantity & cq = this->conserved_quantities;
    this->fs.gas_state.rho = cq[cq.rho()];
    this->fs.velocity.x = cq[cq.momentum()] / cq[cq.rho()];
    this->fs.velocity.y = cq[cq.momentum()+1] / cq[cq.rho()];
    double ke = 0.5*(fs.velocity.x*fs.velocity.x + fs.velocity.y*fs.velocity.y);
    this->fs.gas_state.u = cq[cq.energy()] - ke;
    this->_config.g_model().update_from_rhou(fs.gas_state);
}

Vector3 & Cell::get_pos(){
    return this->_pos;
}

const unsigned int Cell::number_vertices() const {
    return this->_vertices.size();
}

const std::vector<Vertex * > & Cell::vertices() const {
    return this->_vertices;
}

const CellShape::CellShape Cell::get_shape() const {
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
