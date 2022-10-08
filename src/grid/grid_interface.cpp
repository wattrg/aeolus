#include "grid_interface.h"

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
    Cell * right_cell = this->_left_cell;
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
