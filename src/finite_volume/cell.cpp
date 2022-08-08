#include "cell.h"

Cell::Cell(Interface * face, bool valid) : _valid_cell(valid) {
    this->_interfaces.push_back(CellFace(*face, false)); 
}

Cell::Cell(std::vector<Vertex*> vertices, std::vector<Interface*> interfaces, unsigned int id, bool valid)
 : _vertices(vertices), _valid_cell(valid), _id(id)
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
    bool inward_interface;
    for (Interface * interface : interfaces) {
        inward_interface = interface->attach_cell(*this);
        this->_interfaces.push_back(CellFace(*interface, inward_interface)); 
    }
}

void Cell::_compute_shape(){
    switch (this->_vertices.size()){
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
    double surface_integral = 0.0;
    int n_conserved = this->conserved_quantities.n_conserved_quantities();
    for (int i = 0; i < n_conserved; i++){
        for (CellFace face : this->_interfaces){
            Interface * fvface = face.interface;
            double area = face.inwards * fvface->area(); 
            surface_integral += area * fvface->flux().conserved_quantities[i];
        }
    }
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
