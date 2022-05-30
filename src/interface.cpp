#include "interface.h"

Interface::Interface(std::vector<Vertex> vertices) : _vertices(vertices) {
    // TODO: check if vertices are co-planar
}

std::ostream& operator << (std::ostream& os, const Interface interface){
    os << "Interface(";
    for (Vertex vertex : interface._vertices){
        os << vertex << ", ";
    }
    os << ")";
    return os;
}

bool Interface::is(Interface & other) {
    bool is_same;
    for (Vertex & this_vertex : this->_vertices){
        is_same = true;
        for (Vertex & other_vertex : other._vertices){
            if (!this_vertex.is(other_vertex)) is_same = false;
        }
        if (is_same) return true;
    }
    return false;
}

bool Interface::has_vertex(Vertex & other_vertex){
    bool is_in = false;
    for (Vertex this_vertex : this->_vertices) {
        if (this_vertex.is(other_vertex)) is_in = true;
    }
    return is_in;
}

bool Interface::is(std::vector<Vertex> & vertices){
    bool is_same = true;
    for (Vertex other_vertex : vertices) {
        if (!this->has_vertex(other_vertex)) is_same = false;
    }
    return is_same;
}

void Interface::attach_cell(Cell & cell) {
    Side side = this->_compute_side(cell);
    switch (side){
        case left:
            this->_left = &cell;
            break;
        case right:
            this->_right = &cell;
            break;
        case centre:
            throw std::runtime_error("Cell centre is on the interface");
    }
}

Side Interface::_compute_side(Cell & cell){
    return _compute_side(cell.get_pos());
}

Side Interface::_compute_side(Vector2 & point){
    //       B
    //      /
    //     /
    //    /     C
    //   /
    //  A
    // Assume that the interface is just a line (not valid in 3D)
    double Ax = this->_vertices[0].get_pos().x;
    double Ay = this->_vertices[0].get_pos().y;
    double Bx = this->_vertices[1].get_pos().x;
    double By = this->_vertices[1].get_pos().y;
    double Cx = point.x;
    double Cy = point.y;

    double position = (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax);
    if (std::abs(position) < TOL) return centre;
    else if (position > 0.0) return left;
    else return right;
}
