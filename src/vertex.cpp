#include "vertex.h"

Vertex::Vertex(Vector2 pos): _pos(pos) {}

double Vertex::distance_to(Vertex & other) {
    return this->_pos.distance_to(other._pos);
}

bool Vertex::is_close(Vertex & other) {
    return this->_pos.is_close(other._pos);
}

Vector2 & Vertex::get_pos(){
    return this->_pos;
}

std::ostream& operator << (std::ostream& os, const Vertex vertex) {
    os << "Vertex(";
    os << "x = " << vertex._pos.x << ", ";
    os << "y = " << vertex._pos.y << ")";
    return os;
}
