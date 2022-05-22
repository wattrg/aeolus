#include "interface.h"

Interface::Interface(Vertex & start, Vertex & end) : _vertices{&start, &end} {
}

std::ostream& operator << (std::ostream& os, const Interface interface){
    os << "Interface(";
    os << "start = " << interface._vertices[0]->pos << ", ";
    os << "end = " << interface._vertices[1]->pos << ")";
    return os;
}
