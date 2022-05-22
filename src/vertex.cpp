#include "vertex.h"

Vertex::Vertex(Vector2 pos): pos(pos) {}

std::ostream& operator << (std::ostream& os, const Vertex vertex) {
    os << "Vertex(";
    os << "x = " << vertex.pos.x << ", ";
    os << "y = " << vertex.pos.y << ")";
    return os;
}
