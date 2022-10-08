#include "interface.h"
#include "cell.h"
#include "config.h"

Interface::Interface(Grid::Interface & grid_face, std::vector<Vertex> & vertices)
    : _id(grid_face.id()), 
      _norm(grid_face.norm()), 
      _tan1(grid_face.tan1()), 
      _tan2(grid_face.tan2()), 
      _area(grid_face.area())
{
    // set up the vertices of the interface
    size_t number_vertices = vertices.size();
    //this->_vertices = std::vector<Vertex *>(number_vertices);
    this->_vertices.reserve(number_vertices);
    for (size_t i = 0; i < number_vertices; i++){
        size_t id = grid_face.id();
        this->_vertices.push_back(&vertices[id]);
    }

    // the id's of the cells that will be attached
    this->_left_cell_id = grid_face.get_left_cell_id();
    this->_right_cell_id = grid_face.get_right_cell_id();

    // set up the storage for the flux
    this->_dim = (this->_vertices.size() == 2) ? 2 : 3;
    this->_flux = ConservedQuantity(_dim);
}

void Interface::set_flux_calculator(FluxCalculators flux_calculator){
    switch (flux_calculator){
        case FluxCalculators::hanel:
            this->_compute_flux = &FluxCalculator::hanel;
            break;
        case FluxCalculators::ausmdv:
            this->_compute_flux = &FluxCalculator::ausmdv;
            break;
    }
}

void Interface::copy_left_flow_state(FlowState & fs){
    this->_left.copy(fs);
}

void Interface::copy_right_flow_state(FlowState & fs){
    this->_right.copy(fs);
}

bool Interface::is_on_boundary() const { return this->_is_on_boundary; }

double Interface::area() const {
    return _area;
}

ConservedQuantity & Interface::flux() {
    return _flux;
}

Interface::~Interface(){}

void Interface::compute_flux(){
    this->_transform_flowstate_to_local_frame();
    this->_compute_flux(this->_left, this->_right, this->_flux);
    this->_transform_flux_to_global_frame();
}

void Interface::_transform_flux_to_global_frame(){
    unsigned int momentum = this->_flux.momentum();
    double p_x = 0.0, p_y = 0.0, p_z = 0.0;
    p_x = this->_norm.x*this->_flux[momentum] + this->_tan1.x*this->_flux[momentum+1];
    p_y = this->_norm.y*this->_flux[momentum] + this->_tan1.y*this->_flux[momentum+1];
    if (this->_dim == 3){
        p_x += this->_tan2.x*this->_flux[momentum+2];
        p_y += this->_tan2.y*this->_flux[momentum+2];
        p_z = this->_norm.z*this->_flux[momentum] + this->_tan1.z*this->_flux[momentum+1] + this->_tan2.z*this->_flux[momentum+2];
    }
    this->_flux[momentum] = p_x;
    this->_flux[momentum+1] = p_y;
    if (this->_dim == 3){
        this->_flux[momentum+2] = p_z;
    }
}

void Interface::_transform_flowstate_to_local_frame(){
    this->_left.velocity.transform_to_local_frame(this->_norm, this->_tan1, this->_tan2);
    this->_right.velocity.transform_to_local_frame(this->_norm, this->_tan1, this->_tan2);
}

void Interface::_transform_flowstate_to_global_frame(){
    this->_left.velocity.transform_to_global_frame(this->_norm, this->_tan1, this->_tan2);
    this->_right.velocity.transform_to_global_frame(this->_norm, this->_tan1, this->_tan2);
}

Cell & Interface::get_valid_cell(){
    bool left = this->get_left_cell()->is_valid();
    bool right = this->get_right_cell()->is_valid();

    if (left && right) throw std::runtime_error("Both cells are valid");
    return (left) ? *this->get_left_cell() : *this->get_right_cell();
}

std::ostream& operator << (std::ostream& os, const Interface interface){
    os << "Interface(";
    os << "vertices = [";
    for (Vertex * vertex : interface._vertices){
        os << *vertex << ", ";
    }
    os << "], ";
    os << "left = " << interface._left_cell << ", ";
    os << "right = " << interface._right_cell << ", ";
    os << "on boundary = " << interface._is_on_boundary << ", ";
    os << ")";

    return os;
}

Cell * Interface::get_left_cell() {
    // return reference to the left cell
    return this->_left_cell;
}
Cell * Interface::get_right_cell() {
    // return reference to the right cell
    return this->_right_cell;
}

bool Interface::is(Interface & other) {
    // check if `other` is the same interface as this one
    bool is_same;
    for (Vertex * this_vertex : this->_vertices){
        is_same = true;
        for (Vertex * other_vertex : other._vertices){
            if (!this_vertex->is(*other_vertex)) is_same = false;
        }
        if (is_same) return true;
    }
    return false;
}

bool Interface::has_vertex(Vertex & other_vertex){
    // check if `other_vertex` is in this interface
    bool is_in = false;
    for (Vertex * this_vertex : this->_vertices) {
        if (this_vertex->is(other_vertex)) is_in = true;
    }
    return is_in;
}

bool Interface::is(std::vector<Vertex *> & vertices){
    // check `vertices` form this interface
    for (Vertex * other_vertex : vertices) {
        if (!this->has_vertex(*other_vertex)) return false;
    }
    return true;
}


void Interface::attach_cell_left(Cell & cell){
    this->_left_cell = &cell;
}

void Interface::attach_cell_right(Cell & cell){
    this->_right_cell = &cell;
}

void Interface::mark_on_boundary(std::string tag) {
    this->_is_on_boundary = true;
    this->_boundary_tag = tag;
}

