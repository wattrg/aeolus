#include "interface.h"

Interface::Interface(std::vector<Vertex *> vertices, GlobalConfig & config, unsigned int id )
    : _vertices(vertices), _my_config(config), _id(id)
{
    // TODO: check if vertices are co-planar

    _left = new FlowState();
    _right = new FlowState();

    if (this->_vertices.size() == 2){
        // signals two dimensional grid
        this->_tan1 = this->_vertices[1] - this->_vertices[0];
        double length = this->_tan1.length();
        this->_tan1.scale(1.0 / length);
        this->_tan2 = Vector3(0.0, 0.0, 1.0);
        this->_norm = this->_tan1.cross(this->_tan2);
        std::vector<Vector3> points {vertices[0]->get_pos(), vertices[1]->get_pos()};
        this->_area = line_area(points);
    }
    else {
        std::runtime_error("unsupported number of dimensions");
    }
}

const double Interface::area() const {
    return _area;
}

const ConservedQuantity & Interface::flux() const {
    return _flux;
}

Interface::~Interface(){
    delete _left;
    delete _right;
}

void Interface::compute_flux(){
    this->_transform_flowstate_to_local_frame();
    _compute_flux(*this->_left, *this->_right, this->_flux);
    this->_transform_flux_to_global_frame();
}

void Interface::_transform_flux_to_global_frame(){
    double * momentum = this->_flux.momentum();
    double p_x = 0.0, p_y = 0.0, p_z = 0.0;
    p_x = this->_norm.x*momentum[0] + this->_tan1.x*momentum[1];
    p_y = this->_norm.y*momentum[0] + this->_tan1.y*momentum[1];
    if (this->_my_config.dimensions == 3){
        p_x += this->_tan2.x*momentum[2];
        p_y += this->_tan2.y*momentum[2];
        p_z = this->_norm.z*momentum[0] + this->_tan1.z*momentum[1] + this->_tan2.z*momentum[2];
    }
    momentum[0] = p_x;
    momentum[1] = p_y;
    if (this->_my_config.dimensions == 3){
        momentum[2] = p_z;
    }
}

void Interface::_transform_flowstate_to_local_frame(){
    this->_left->velocity.transform_to_local_frame(this->_norm, this->_tan1, this->_tan2);
    this->_right->velocity.transform_to_local_frame(this->_norm, this->_tan1, this->_tan2);
}

void Interface::_transform_flowstate_to_global_frame(){
    this->_left->velocity.transform_to_global_frame(this->_norm, this->_tan1, this->_tan2);
    this->_right->velocity.transform_to_global_frame(this->_norm, this->_tan1, this->_tan2);
}

Cell & Interface::get_valid_cell(){
    bool left = this->get_left_cell().is_valid();
    bool right = this->get_right_cell().is_valid();

    if (left && right) throw std::runtime_error("Both cells are valid");
    return (left) ? this->get_left_cell() : this->get_right_cell();
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

Cell & Interface::get_left_cell() {
    // return reference to the left cell
    return *this->_left_cell;
}
Cell & Interface::get_right_cell() {
    // return reference to the right cell
    return *this->_right_cell;
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
    // chceck `vertices` form this interface
    bool is_same = true;
    for (Vertex * other_vertex : vertices) {
        if (!this->has_vertex(*other_vertex)) is_same = false;
    }
    return is_same;
}

bool Interface::attach_cell(Cell & cell) {
    // Attach this cell to the left or right of the interface
    Side side = this->_compute_side(cell);
    switch (side){
        case left:
            this->_left_cell = &cell;
            return true;
        case right:
            this->_right_cell = &cell;
            return false;
        case centre:
            throw std::runtime_error("Cell centre is on the interface");
    }
    throw std::runtime_error("Shouldn't get here");
}

void Interface::mark_on_boundary(std::string tag) {
    this->_is_on_boundary = true;
    this->_boundary_tag = tag;
}

Side Interface::_compute_side(Cell & cell){
    return _compute_side(cell.get_pos());
}

Side Interface::_compute_side(Vector3 & point){
    //       B
    //      /
    //     /
    //    /     C
    //   /
    //  A
    // Assume that the interface is just a line (not valid in 3D)
    double Ax = this->_vertices[0]->get_pos().x;
    double Ay = this->_vertices[0]->get_pos().y;
    double Bx = this->_vertices[1]->get_pos().x;
    double By = this->_vertices[1]->get_pos().y;
    double Cx = point.x;
    double Cy = point.y;

    double position = (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax);
    if (std::abs(position) < TOL) return centre;
    else if (position > 0.0) return left;
    else return right;
}
