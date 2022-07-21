#include "grid_io.h"

GridIO::GridIO(GridInput * input, GridOutput * output)
    : _grid_input(input), _grid_output(output) {}


GridIO::GridIO(GridFormat::GridFormats input, GridFormat::GridFormats output)
{
    // the input format
    switch (input){
        case GridFormat::su2:
            _grid_input = new Su2GridInput();
            break;
        case GridFormat::none:
            break;
        default:
            throw std::runtime_error("Unknown grid input format");
    }

    // the output format
    switch (output){
        case GridFormat::su2:
            break;
        case GridFormat::none:
            break;
        default:
            throw std::runtime_error("Unknown grid output format");
    }
}

void GridIO::read_grid(const char * file_name, FluidBlock & fluid_block){
    if (_grid_input){
        _grid_input->read_grid(file_name, fluid_block);
    }
    else {
        throw std::runtime_error("Grid input not initialised");
    }
}

void GridIO::write_grid(const char * file_name, FluidBlock & fluid_block){
    if (_grid_output){
        _grid_output->write_grid(file_name, fluid_block);
    }
    else {
        throw std::runtime_error("Grid output not initialised");
    }
}

// some useful stuff
struct Element {

    ElementShape::ElementShape shape;
    std::vector<int> vertices;

    Element(ElementShape::ElementShape shape, std::vector<int> vertices)
        : shape(shape), vertices(vertices)
    {};
};

ElementShape::ElementShape int_to_element_shape(int shape){
    switch (shape) {
        case 3:
            return ElementShape::Line;
        case 9:
            return ElementShape::Quad;
        default:
            throw std::runtime_error("Only Quads and Lines supported for the moment");
    }
}

int element_shape_to_number_vertices(ElementShape::ElementShape shape){
    switch (shape) {
        case ElementShape::Line:
            return 2;
        case ElementShape::Quad:
            return 4;
        default:
            throw std::runtime_error("Invalid ElementShape");
    }
}

std::string read_string(std::string str){
    // Read a string value from a string of the form:
    // "NAME = 51"
    // Discard anything before the equal sign
    size_t sep = str.find("=");
    str = str.substr(sep+1);
    sep = str.find(" ");
    if (sep != std::string::npos){
        str = str.substr(sep);
    }
    return str;
}

int read_integer(std::string str){
    return std::stoi(read_string(str));
}

Element read_element(std::string line) {
    // read the element type
    int pos = line.find(" ");
    int elem_shape = std::stoi(line.substr(0, pos));
    ElementShape::ElementShape shape = int_to_element_shape(elem_shape);
    int n_vertices = element_shape_to_number_vertices(shape);
    // discard the element type
    line = line.substr(pos+1, std::string::npos);

    // read the vertices
    int vertex;
    std::vector<int> element_vertices{};
    for (int i_vertex=0; i_vertex < n_vertices; i_vertex++){
        pos = line.find(" ");
        vertex = std::stoi(line.substr(0, pos));
        line = line.substr(pos+1, std::string::npos);
        element_vertices.push_back(vertex);
    }
    return Element(shape, element_vertices);
}

void Su2GridInput::read_grid(const char * file_name, FluidBlock & fluid_block){
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
    fluid_block.fb_config.dimensions = n_dim;

    // Read the vertices
    std::getline(su2_file, line);
    int n_points = read_integer(line);
    for ( int vertex_id=0; vertex_id < n_points; vertex_id++ ){
        double x, y;
        std::getline(su2_file, line);
        int sep = line.find(" ");
        x = std::stod(line.substr(0, sep));
        y = std::stod(line.substr(sep));
        fluid_block._vertices.push_back(new Vertex(Vector3(x,y), vertex_id));
    }

    // Read the cells
    std::getline(su2_file, line);
    if (line.find("NELEM") != 0) {
        throw std::runtime_error("Could not find number of cells");
    }
    int n_elems = read_integer(line);
    for ( int index=0; index < n_elems; index++ ) {
        std::getline(su2_file, line);
        Element element = read_element(line);
        if (element.shape == ElementShape::Line){
            throw std::runtime_error("A cell cannot be a line");
        }
        int n_vertices = element_shape_to_number_vertices(element.shape);

        // read each vertex for this cell
        std::vector<Vertex *> cell_vertices{};
        for (int vertex : element.vertices){
            cell_vertices.push_back(fluid_block._vertices[vertex]);
        }

        // make the interfaces for each cell
        std::vector<Interface *> cell_interfaces;
        std::vector<Vertex *> interface_vertices;

        for ( int i_vertex=0; i_vertex < n_vertices-1; i_vertex++) {
            interface_vertices.assign({cell_vertices[i_vertex], cell_vertices[i_vertex+1]});
            Interface * interface = fluid_block._add_interface(interface_vertices);
            cell_interfaces.push_back(interface);
        }
        // the last interface wraps around, so we can't use the above pattern
        // instead we just hard code the closing interface
        interface_vertices.assign({cell_vertices[n_vertices-1], cell_vertices[0]});
        Interface * interface = fluid_block._add_interface(interface_vertices);
        cell_interfaces.push_back(interface);
        fluid_block._cells.push_back(new Cell(cell_vertices, cell_interfaces));
    }

    // read the boundary conditions
    std::getline(su2_file, line);
    if (line.find("NMARK") != 0){
        throw std::runtime_error("Could not find number of boundaries");
    }
    int n_boundaries = read_integer(line);
    for (int i_boundary = 0; i_boundary < n_boundaries; i_boundary++){
        std::getline(su2_file, line);
        // first comes the tag
        std::string tag = read_string(line);
        // next comes the number of elements on this boundary
        std::getline(su2_file, line);
        if (line.find("MARKER_ELEMS") != 0){
            throw std::runtime_error("Could not find the number of elements on boundary");
        }
        int n_elements = read_integer(line);
        // finally, the actual elements
        for (int i_element = 0; i_element < n_elements; i_element++){
            std::getline(su2_file, line);
            Element element = read_element(line);
            if (element.shape != ElementShape::Line){
                throw std::runtime_error("Boundary element should only be a line");
            }
            std::vector<Vertex *> boundary_vertices = {};
            for (int vertex_indx : element.vertices){
                boundary_vertices.push_back(fluid_block._vertices[vertex_indx]);
            }
            Interface * interface = fluid_block._find_interface(boundary_vertices);
            if (!interface) {
                for (int vertex_indx : element.vertices) {
                    std::cout << vertex_indx << ", ";
                }
                std::cout << "\n";
                throw std::runtime_error("Could not find the interface on boundary");
            }
            interface->mark_on_boundary(tag);
        }
    }
    // All done
    su2_file.close();
}
