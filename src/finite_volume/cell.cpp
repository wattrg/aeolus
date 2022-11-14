#include "cell.h"
#include "../util/unused.h"

Cell::Cell(Interface & face, int id, bool valid) 
    : _valid_cell(valid),
      _id(id)
{
    this->_interfaces[0] = CellFace(face, false); 
}

Cell::Cell(Grid::Cell & grid_cell, std::vector<Interface> & interfaces)
    : _pos(grid_cell.get_pos()), 
      _shape(grid_cell.get_shape()),
      _valid_cell(true),
      _volume(grid_cell.volume()),
      _id(grid_cell.id())
{
    std::vector<Grid::CellFace> grid_interfaces = grid_cell.interfaces();
    std::vector<Grid::Vertex *> grid_vertices = grid_cell.vertices();

    this->_number_interfaces = grid_interfaces.size();
    this->_number_vertices = grid_vertices.size();

    // assign pointers to interfaces and vertices
    for (int i = 0; i < this->_number_vertices; i++){
        size_t id = grid_vertices[i]->id();
        this->_vertices[i] = id;
    }

    for (int i = 0; i < this->_number_interfaces; i++){
        Grid::CellFace grid_interface = grid_interfaces[i];
        size_t id = grid_interface.interface->id();
        this->_interfaces[i] = CellFace(interfaces[id], grid_interface.outwards);
    }
}


#ifdef GPU
#pragma omp declare target
#endif
double Cell::volume() const {return this->_volume;}
#ifdef GPU
#pragma omp end declare target
#endif


Vector3 & Cell::get_pos(){
    return this->_pos;
}

unsigned int Cell::number_vertices() const {
    return this->_number_vertices;
}

int * Cell::vertices() {
    return this->_vertices;
}

Grid::CellShape Cell::get_shape() const {
    return _shape; 
}

std::string Cell::to_string() const {
    std::string str = "Cell(";
    str.append(this->_pos.to_string());
    // str.append(this->fs.to_string());
    str.append(", ");
    str.append("vertices = [");
    for (int i = 0; i < this->_number_vertices; i++){
        str.append(std::to_string(i));
    }
    str.append("])");
    return str;
}

std::ostream& operator << (std::ostream& os, const Cell cell){
    os << cell.to_string();
    return os;
}
