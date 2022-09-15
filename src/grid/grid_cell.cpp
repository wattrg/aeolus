#include "grid_cell.h"

Grid::Cell::Cell(std::vector<Vertex *> vertices,
                 std::vector<Interface *> interfaces,
                 unsigned int id)
    : _vertices(vertices), _id(id)
{
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

    this->_compute_shape();
    this->_compute_volume();

    //  attach the cell to the interfaces
    bool outward_interface;
    for (Interface * interface : interfaces) {
        outward_interface = interface->attach_cell(*this);
        this->_interfaces.push_back(CellFace(*interface, outward_interface)); 
    }
}

std::string Grid::Cell::to_string() const {
    std::string str = "Cell(";
    str.append(this->_pos.to_string());
    str.append(", ");
    str.append("vertices = [");
    for (Vertex * vertex : this->_vertices){
        str.append(vertex->to_string());
    }
    str.append("])");
    return str;
}

void Grid::Cell::_compute_shape(){
    switch (this->_vertices.size()){
        case 3:
            this->_shape = CellShape::Triangle;
            break;
        case 4:
            this->_shape = CellShape::Quad;
            break;
        default:
            throw std::runtime_error("unknown cell shape");
    }
}

void Grid::Cell::_compute_volume(){
    switch (this->_shape){
        case CellShape::Triangle:
            {
            std::vector<Vector3> points;
            points.reserve(3);
            for (Vertex * vertex : this->_vertices){
                points.push_back(vertex->get_pos());
            }
            this->_volume = triangle_area(points);
            break;
            }
        case CellShape::Quad:
            {
            std::vector<Vector3> points;
            points.reserve(4);
            for (Vertex * vertex : this->_vertices){
                points.push_back(vertex->get_pos());
            }
            this->_volume = quad_area(points);
            break;
            }
        default:
            throw std::runtime_error("unknown cell shape in volume calculation");
    }
}
