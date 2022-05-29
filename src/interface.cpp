#include "interface.h"

Interface::Interface(Vertex & start, Vertex & end) : _vertices{&start, &end} {
}

std::ostream& operator << (std::ostream& os, const Interface interface){
    os << "Interface(";
    os << "start = " << interface.get_start_vertex().get_pos() << ", ";
    os << "end = " << interface.get_end_vertex().get_pos() << ")";
    return os;
}

Vertex & Interface::get_start_vertex() const {
    return *_vertices[0];
}

Vertex & Interface::get_end_vertex() const {
    return *_vertices[1];
}

bool Interface::is_close(Interface other) {
    bool start_close = this->get_start_vertex().is_close(other.get_start_vertex());
    bool end_close = this->get_start_vertex().is_close(other.get_end_vertex());
    return start_close && end_close;
}
