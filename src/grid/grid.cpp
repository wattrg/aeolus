#include "grid.h"

Grid::Grid::~Grid() {
    for (Vertex *v : this->_vertices){
        delete v;
    }
    for (Interface *f : this->_interfaces){
        delete f;
    }
    for (Cell *cell : this->_cells){
        delete cell;
    }
}

Grid::GridFormats Grid::extension_to_grid_format(std::string extension){
    if (extension == "su2") return GridFormats::su2;
    throw std::runtime_error("Unknown grid format");
}

Grid::Grid::Grid (std::string file_name){
    // the file extension
    size_t ext_start = file_name.find_last_of('.');
    std::string extension = file_name.substr(ext_start+1);
    //GridFormats grid_format = extension_to_grid_format(extension);

    GridIO grid_io = GridIO();
    grid_io.read_grid(file_name, *this);
}

void Grid::Grid::set_grid(std::vector<Vertex *> vertices, 
                          std::vector<Interface *> interfaces,
                          std::vector<Cell *> cells,
                          std::map<std::string, std::vector<Interface *>> bcs) {
    this->_vertices = vertices;
    this->_interfaces = interfaces;
    this->_cells = cells;
    this->_bcs = bcs;
}

std::string Grid::Grid::to_string(){
    std::string str = "Grid(";
    str.append("n_interfaces = ");
    str.append(std::to_string(this->_interfaces.size()));
    str.append(", n_cells = ");
    str.append(std::to_string(this->_cells.size()));
    str.append(", cells = [");
    for (Cell * cell : this->_cells){
        str.append(cell->to_string());
        str.append(",\n\n");
    }
    str.append(")");
    return str;
}
