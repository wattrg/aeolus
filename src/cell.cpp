#include "cell.h"

Cell::Cell(std::vector<Vertex> vertices, std::vector<Interface> interfaces)
 : _interfaces(interfaces), _vertices(vertices)
{
}

std::ostream& operator << (std::ostream& os, const Cell cell){
    os << "Cell(";
    os << cell.fs << ", ";
    os << "vertices = [";
    for (Vertex vertex : cell._vertices) {
        os << vertex << ", ";
    }
    os << "], ";
    os << "interfaces = [";
    for (Interface interface : cell._interfaces) {
        os << interface << ", ";
    }
    os << "]";
    os << ")";
    return os;
}
