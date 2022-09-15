#include "vertex.h"

Vertex::Vertex(Grid::Vertex & grid_vertex) 
    : _pos(grid_vertex.get_pos()), _id(grid_vertex.id()) {}

double Vertex::distance_to(Vertex & other) {
    return this->_pos.distance_to(other._pos);
}

bool Vertex::is(Vertex & other) {
    return this->_id == other._id;
}

int Vertex::id() const {
    return this->_id;
}

const Vector3 & Vertex::get_pos() const {
    return this->_pos;
}

std::string Vertex::to_string() const {
    std::string str = "Vertex(";
    str.append("id = ");
    str.append(std::to_string(this->_id));
    str.append(",");
    str.append("position = ");
    str.append(this->_pos.to_string());
    str.append(")");
    return str;
}

std::ostream& operator << (std::ostream& os, const Vertex vertex) {
    os << vertex.to_string();
    return os;
}
