#include "grid_io.h"
#include "su2.h"

GridIO::GridIO() {
    this->_grid_input = new Su2GridInput();
    this->_grid_output = nullptr;
}

//GridIO::GridIO(Grid::GridFormats input, Grid::GridFormats output)
//{
//    // the input format
//    switch (input){
//        case Grid::GridFormats::su2:
//            _grid_input = new Su2GridInput();
//            break;
//        case Grid::GridFormats::none:
//            break;
//        default:
//            throw std::runtime_error("Unknown grid input format");
//    }
//
//    // the output format
//    switch (output){
//        case Grid::GridFormats::su2:
//            break;
//        case Grid::GridFormats::none:
//            break;
//        default:
//            throw std::runtime_error("Unknown grid output format");
//    }
//}

GridOutput::~GridOutput(){};

GridIO::~GridIO(){
    if (_grid_input) delete _grid_input;
    if (_grid_output) delete _grid_output;
}

void GridIO::read_grid(std::string file_name, Grid::Grid & grid){
    if (_grid_input){
        _grid_input->read_grid(file_name);
        grid.set_grid(_grid_input->vertices(), 
                      _grid_input->interfaces(), 
                      _grid_input->cells(),
                      _grid_input->bcs());
    }
    else {
        throw std::runtime_error("Grid input not initialised");
    }
}
//
//void GridIO::write_grid(std::string file_name, Grid::Grid & grid){
//    if (_grid_output){
//        _grid_output->write_grid(file_name, grid);
//    }
//    else {
//        throw std::runtime_error("Grid output not initialised");
//    }
//}


Grid::ElementShape int_to_element_shape(int shape){
    switch (shape) {
        case 3:
            return Grid::ElementShape::Line;
        case 5:
            return Grid::ElementShape::Triangle;
        case 9:
            return Grid::ElementShape::Quad;
        default:
            throw std::runtime_error("Only Quads and Lines supported for the moment");
    }
}

int element_shape_to_number_vertices(Grid::ElementShape shape){
    switch (shape) {
        case Grid::ElementShape::Line:
            return 2;
        case Grid::ElementShape::Triangle:
            return 3;
        case Grid::ElementShape::Quad:
            return 4;
        default:
            throw std::runtime_error("Invalid ElementShape");
    }
}

std::string trim_whitespace(std::string str){
    // remove whitespace from the beginning and end
    // trim the left
    size_t start = str.find_first_not_of(" ");
    str = (start == std::string::npos) ? "" : str.substr(start);

    // trim the right
    size_t end = str.find_last_not_of(" ");
    str = (end == std::string::npos) ? "" : str.substr(0, end+1);

    return str;
}

std::string read_string(std::string str){
    // Read a string value from a string of the form:
    // "NAME = 51"
    // Discard anything before the equal sign
    size_t sep = str.find("=");
    str = str.substr(sep+1);
    // trim any white space from around the value
    str = trim_whitespace(str);
    return str;
}

int read_integer(std::string str){
    str = read_string(str);
    return std::stoi(str);
}

Grid::Element read_element(std::string line) {
    // read the element type
    int pos = line.find(" ");
    int elem_shape = std::stoi(line.substr(0, pos));
    Grid::ElementShape shape = int_to_element_shape(elem_shape);
    int n_vertices = element_shape_to_number_vertices(shape);

    // discard the element type
    line = line.substr(pos+1, std::string::npos);

    // read the vertices
    int vertex;
    std::vector<int> element_vertices{};
    for (int i_vertex=0; i_vertex < n_vertices; i_vertex++){
        line = trim_whitespace(line);
        pos = line.find(" ");
        vertex = std::stoi(line.substr(0, pos));
        line = line.substr(pos+1, std::string::npos);
        element_vertices.push_back(vertex);
    }
    return Grid::Element(shape, element_vertices);
}

Grid::Interface * GridInput::_find_interface(std::vector<Grid::Vertex *> vertices){
    // Return a pointer to the interface with verteices `vertices`
    for (Grid::Interface * interface : this->_interfaces){
        if (interface->is(vertices)) return interface;
    }
    return nullptr;
}

Grid::Interface * GridInput::_add_interface(std::vector<Grid::Vertex *> vertices){
    // loop through all the interfaces we have so far, checking
    // if the this new one is already in our collection of them
    for (Grid::Interface * interface : this->_interfaces){
        if (interface->is(vertices)){
            //  the interface already exists, so we'll return
            // a reference to the interface
            return interface;
        }
    }
    // the interface doesn't exist, so we'll create a new one,
    // add it to the list, and return a reference to it
    Grid::Interface * interface = new Grid::Interface(vertices, this->_interfaces.size());
    this->_interfaces.push_back(interface);
    return interface;
}
