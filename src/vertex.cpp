#include "vertex.h"

Vertex::Vertex(Vector2 pos, int id): _pos(pos), _id(id) {}

double Vertex::distance_to(Vertex & other) {
    return this->_pos.distance_to(other._pos);
}

bool Vertex::is(Vertex & other) {
    return this->_id == other._id;
}

Vector2 & Vertex::get_pos(){
    return this->_pos;
}

std::ostream& operator << (std::ostream& os, const Vertex vertex) {
    os << "Vertex(";
    os << "id = " << vertex._id << ", ";
    os << "x = " << vertex._pos.x << ", ";
    os << "y = " << vertex._pos.y << ")";
    return os;
}
