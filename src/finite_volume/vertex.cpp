#include "vertex.h"

Vertex::Vertex(Vector3 pos, int id): _pos(pos), _id(id) {}

double Vertex::distance_to(Vertex & other) {
    return this->_pos.distance_to(other._pos);
}

bool Vertex::is(Vertex & other) {
    return this->_id == other._id;
}

const int Vertex::id() const {
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
