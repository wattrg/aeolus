#include "grid_interface.h"
#include <algorithm>

Grid::Interface::Interface(std::vector<Vertex *> vertices, unsigned int id)
    : _vertices(vertices), _id(id)
{
    // TODO: check if vertices are co-planar
    if (this->_vertices.size() == 2){
        // two dimensional grid
        this->_t1 = this->_vertices[1]->get_pos() - this->_vertices[0]->get_pos();
        double length = this->_t1.length();
        this->_t1.scale(1.0 / length);
        this->_t2 = Vector3(0.0, 0.0, 1.0);
        this->_n = this->_t1.cross(this->_t2);
        std::vector<Vector3> points {vertices[0]->get_pos(), vertices[1]->get_pos()};
        this->_area = line_area(points);
    }
    else {
        throw std::runtime_error("Unsported number of dimensions");
    }
}

// Grid::Interface::Interface(Interface &other){
//     this->_vertices = other.vertices();
//     this->_id = other.id();
//     this->_left_cell = other.get_left_cell();
//     this->_right_cell = other.get_right_cell();
//     this->_n = other.norm();
//     this->_t1 = other.tan1();
//     this->_t2 = other.tan2();
//     this->_area = other.area();
//     this->_is_on_boundary = other.is_on_boundary();
//     this->_boundary_tag = other.boundary_tag();
// }

int Grid::Interface::get_left_cell_id() {
    Cell * left_cell = this->_left_cell;
    if (left_cell){
        return left_cell->id();
    }
    else {
        return -1;
    }
}
int Grid::Interface::get_right_cell_id() {
    Cell * right_cell = this->_right_cell;
    if (right_cell){
        return right_cell->id();
    }
    else {
        return -1;
    }
}

bool Grid::Interface::is(std::vector<Vertex *> & vertices){
    for (Vertex * other_vertex : vertices){
        bool has_vertex = false;
        for (Vertex * this_vertex : this->_vertices) {
            if (*other_vertex == *this_vertex) {
                has_vertex = true;
                break;
            }
        }
        if (!has_vertex) return false;
    }
    return true;
}

bool Grid::Interface::attach_cell(Cell & cell){
    Side side = this->_compute_side(cell);
    switch (side){
        case Side::left:
            this->_left_cell = &cell;
            return true;
        case Side::right:
            this->_right_cell = &cell;
            return false;
        case Side::centre:
            throw std::runtime_error("Cell centre is on the interface");
    }
    throw std::runtime_error("unreachable");
}

void Grid::Interface::attach_cell_left(Cell & cell){
    this->_left_cell = &cell;
}

void Grid::Interface::attach_cell_right(Cell & cell){
    this->_right_cell = &cell;
}

void Grid::Interface::mark_on_boundary(std::string tag){
    this->_is_on_boundary = true;
    this->_boundary_tag = tag;
}


Side Grid::Interface::_compute_side(Vector3 & point){
    //       B
    //      /
    //     /
    //    /     C
    //   /
    //  A
    // Assume that the interface is just a line (not valid in 3D)
    double Ax = this->_vertices[0]->get_pos().x;
    double Ay = this->_vertices[0]->get_pos().y;
    double Bx = this->_vertices[1]->get_pos().x;
    double By = this->_vertices[1]->get_pos().y;
    double Cx = point.x;
    double Cy = point.y;

    double position = (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax);
    if (std::abs(position) < TOL) return Side::centre;
    if (position > 0.0) return Side::left;
    return Side::right;
}

Side Grid::Interface::_compute_side(Cell & cell){
    return this->_compute_side(cell.get_pos());
}

std::string Grid::Interface::to_string() const {
    std::string str = "Interface(v0 = ";
    str.append(std::to_string(this->_vertices[0]->id()));
    str.append(", v1 =");
    str.append(std::to_string(this->_vertices[1]->id()));
    str.append(")");
    return str;
}

std::string hash(std::vector<int> vertex_ids){
    // sort the ids from highest to lowest, and concatenate as strings
    std::sort(vertex_ids.begin(), vertex_ids.end(), std::greater<int>());
    std::string result = "";
    for (int vertex_id : vertex_ids) {
        result.append(std::to_string(vertex_id));
        result.append(",");
    }
    return result;
}

Grid::Interface *
InterfaceCollection::add_or_retrieve(std::vector<Grid::Vertex *> vertices){
    // create vector of IDS
    std::vector<int> ids;
    for (Grid::Vertex * vertex : vertices){
        ids.push_back(vertex->id());
    }

    std::string interface_hash = hash(ids);
    if (this->_interfaces.find(interface_hash) == this->_interfaces.end()) {
        // the interface isn't in the hash map already
        // so we construct a new interface and add it to the collection
        int id = this->_interfaces.size();
        Grid::Interface interface = Grid::Interface(vertices, id);
        this->_interfaces.insert({interface_hash, interface});
        this->_id_to_hash.insert({id, interface_hash});
    }
    
    // return a pointer to the interface
    return &this->_interfaces.at(interface_hash);
}

std::vector<Grid::Interface *> InterfaceCollection::interfaces() {
    std::vector<Grid::Interface *> interfaces;
    for (std::map<std::string, Grid::Interface>::iterator it = _interfaces.begin(); it != _interfaces.end(); ++it) {
        Grid::Interface * interface = new Grid::Interface(it->second); 
        interfaces.push_back(interface);
    }
    return interfaces;
}
