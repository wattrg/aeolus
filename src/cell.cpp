#include "cell.h"

Cell::Cell(std::vector<Vertex*> vertices, std::vector<Interface*> interfaces)
 : _interfaces(interfaces), _vertices(vertices)
{
    // TODO do we need to have vertices passed to the constructor?
    // we could get away with just the interfaces

    // find the centroid of the cell
    double centre_x = 0.0;
    double centre_y = 0.0;
    int num_vertices = vertices.size();
    for (Vertex * vertex : _vertices){
        centre_x += vertex->get_pos().x;
        centre_y += vertex->get_pos().y;
    }
    centre_x /= num_vertices;
    centre_y /= num_vertices;
    _pos = Vector2(centre_x, centre_y);

    //  attach the cell to the interfaces
    for (Interface * interface : _interfaces) {
        interface->attach_cell(*this);
    }
}

Vector2 & Cell::get_pos(){
    return this->_pos;
}

std::ostream& operator << (std::ostream& os, const Cell cell){
    os << "Cell(";
    os << cell.fs << ", ";
    os << "vertices = [";
    for (Vertex * vertex : cell._vertices) {
        os << *vertex << ", ";
    }
    os << "], ";
    os << "interfaces = [";
    for (Interface * interface : cell._interfaces) {
        os << *interface << ", ";
    }
    os << "]";
    os << ")";
    return os;
}
