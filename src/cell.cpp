#include "cell.h"

Cell::Cell(std::vector<Vertex*> vertices, std::vector<Interface*> interfaces)
 : _vertices(vertices)
{
    // TODO do we need to have vertices passed to the constructor?
    // we could get away with just the interfaces

    //  this->_interfaces = interfaces;
    //

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
    _pos = Vector3(centre_x, centre_y);

    //  attach the cell to the interfaces
    bool inward_interface;
    for (Interface * interface : interfaces) {
        inward_interface = interface->attach_cell(*this);
        this->_interfaces.push_back(CellFace(*interface, inward_interface)); 
    }
}

Vector3 & Cell::get_pos(){
    return this->_pos;
}

std::string Cell::to_string() const {
    std::string str = "Cell(";
    str.append(this->_pos.to_string());
    str.append(this->fs.to_string());
    str.append(", ");
    str.append("vertices = [");
    for (Vertex * vertex : this->_vertices){
        str.append(vertex->to_string());
    }
    str.append("])");
    return str;
}

std::ostream& operator << (std::ostream& os, const Cell cell){
    os << cell.to_string();
    return os;
}
