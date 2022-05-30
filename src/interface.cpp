#include "interface.h"

Interface::Interface(std::vector<Vertex> vertices) : _vertices(vertices) {
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
