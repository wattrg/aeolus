#include "fluid_block.h"

int read_integer(std::string str){
    // Read the integer from a string of the form:
    // "NAME = 51"
    // Discard anything before the equal sign
    size_t sep = str.find("=");
    std::string substr = str.substr(sep+1);
    // if there is leading whitespace, remove it
    sep = substr.find(" ");
    if (sep != std::string::npos){
        substr = substr.substr(sep);
    }
    return std::stoi(substr);
}

FluidBlock::FluidBlock(const char * file_name) {
    std::fstream su2_file;
    su2_file.open(file_name);
    if (!su2_file) {
        throw std::runtime_error("Could not find file");
    }
    std::string line;
    // Read the first line. Assume it is the number of dimensions
    std::getline(su2_file, line);
    if (line.find("NDIME") != 0) {
        throw std::runtime_error("File doesn't begin with NDIME");
    }
    int n_dim = read_integer(line);
    if ( n_dim != 2 ) {
        throw std::runtime_error("Only two dimensions implemented");
    }

    // Read the number of points
    std::getline(su2_file, line);
    int n_points = read_integer(line);
    for ( int index=0; index < n_points; index++ ){
        double x, y;
        std::getline(su2_file, line);
        int sep = line.find(" ");
        x = std::stod(line.substr(0, sep));
        y = std::stod(line.substr(sep));
        Vertex vertex = Vertex(Vector2(x, y));
        _vertices.push_back(vertex);
    }

    // Read the cells
    std::getline(su2_file, line);
    if (line.find("NELEM") != 0) {
        throw std::runtime_error("Could not find number of cells");
    }
    int n_elems = read_integer(line);
    size_t pos;
    for ( int index=0; index < n_elems; index++ ) {
        std::getline(su2_file, line);
        pos = line.find(" ");
        int elem_type = std::stoi(line.substr(0, pos));
        line = line.substr(pos+1, std::string::npos);
        int n_interfaces;
        switch (elem_type){
            case 9:
                n_interfaces = 4;
                break;
            default:
                throw std::runtime_error("Only quads implemented for the moment");
        }

        // read each vertex for this cell
        int vertex;
        std::vector<Vertex> cell_vertices;
        for (int i_vertex=0; i_vertex < n_interfaces; i_vertex++){
            pos = line.find(" ");
            vertex = std::stoi(line.substr(0, pos));
            line = line.substr(pos+1, std::string::npos);
            cell_vertices.push_back(_vertices[vertex]);
        }

        // make the interfaces for each cell
        std::vector<Interface> cell_interfaces;
        for ( int i_vertex=0; i_vertex < n_interfaces-1; i_vertex++) {

            cell_interfaces.push_back(
                Interface(cell_vertices[i_vertex], cell_vertices[i_vertex+1])
            );
        }
        // the last interface wraps around, so we can't use the above pattern
        // instead we just hard code the closing interface
        cell_interfaces.push_back(Interface(cell_vertices[n_interfaces], cell_vertices[0]));
        _cells.push_back(Cell(cell_vertices, cell_interfaces));
    }
    // ignore the boundary conditions for the moment.
    su2_file.close();
}


