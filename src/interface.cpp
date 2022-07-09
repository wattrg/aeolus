#include "interface.h"

Interface::Interface(std::vector<Vertex *> vertices) : _vertices(vertices) {
    // TODO: check if vertices are co-planar
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

void Interface::attach_cell(Cell & cell) {
    // Attach this cell to the left or right of the interface
    Side side = this->_compute_side(cell);
    switch (side){
        case left:
            this->_left_cell = &cell;
            break;
        case right:
            this->_right_cell = &cell;
            break;
        case centre:
            throw std::runtime_error("Cell centre is on the interface");
    }
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
