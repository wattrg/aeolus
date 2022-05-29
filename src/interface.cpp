#include "interface.h"

Interface::Interface(std::vector<Vertex> vertices) : _vertices(vertices) {
}

//std::ostream& operator << (std::ostream& os, const Interface interface){
//    os << "Interface(";
//    os << "start = " << interface.get_start_vertex().get_pos() << ", ";
//    os << "end = " << interface.get_end_vertex().get_pos() << ")";
//    return os;
//}

Vertex & Interface::get_start_vertex() {
    return _vertices[0];
}

Vertex & Interface::get_end_vertex() {
    return _vertices[1];
}

bool Interface::is_close(Interface & other) {
    for (Vertex & this_vertex : this->_vertices){
        for (Vertex & other_vertex : other._vertices){
            if (!this_vertex.is_close(other_vertex)) return false;
        }
    }
    return true;
}

bool Interface::is_close(std::vector<Vertex> & vertices){
    for (Vertex & this_vertex : this->_vertices) {
        for (Vertex other_vertex : vertices) {
            if (!this_vertex.is_close(other_vertex)) return false;
        }
    }
    return true;
}
