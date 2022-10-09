#include "grid_io.h"
//#include "su2.h"

void get_next_line(std::fstream & is, std::string & line){
    // read next valid line of the file (e.g. ignoring blank lines and comments)
    while (true){
        std::getline(is, line);
        if (line.rfind("%", 0) != 0 && !line.empty()){
            break;
        }
    }

    // we've found the next line, do some cleaning up
    // size_t start = line.find_first_not_of(" ");
    // size_t end = line.find_last_not_of(" ");
    // line = line.substr(start, end);
    line = trim_whitespace(line);
}

void Su2GridInput::read_grid(std::string file_name){
    std::fstream su2_file;
    su2_file.open(file_name);
    if (!su2_file) {
        throw std::runtime_error("Could not find file");
    }
    std::string line;
    // Read the first line. Assume it is the number of dimensions
    get_next_line(su2_file, line);
    if (line.find("NDIME") != 0) {
        throw std::runtime_error("File doesn't begin with NDIME");
    }
    int n_dim = read_integer(line);
    if ( n_dim != 2 ) {
        throw std::runtime_error("Only two dimensions implemented");
    }

    // Read the vertices
    get_next_line(su2_file, line);
    int n_points = read_integer(line);
    this->_vertices.reserve(n_points);
    for ( int vertex_id=0; vertex_id < n_points; vertex_id++ ){
        double x, y;
        get_next_line(su2_file, line);
        int sep = line.find(" ");
        x = std::stod(line.substr(0, sep));
        y = std::stod(line.substr(sep));
        this->_vertices.push_back(new Grid::Vertex(Vector3(x,y), vertex_id));
    }

    // Read the cells
    get_next_line(su2_file, line);
    if (line.find("NELEM") != 0) {
        throw std::runtime_error("Could not find number of cells");
    }
    int n_elems = read_integer(line);
    this->_cells.reserve(n_elems);
    for ( int index=0; index < n_elems; index++ ) {
        get_next_line(su2_file, line);
        Grid::Element element = read_element(line);
        if (element.shape == Grid::ElementShape::Line){
            throw std::runtime_error("A cell cannot be a line");
        }
        int n_vertices = element_shape_to_number_vertices(element.shape);

        // read each vertex for this cell
        std::vector<Grid::Vertex *> cell_vertices{};
        for (int vertex : element.vertices){
            cell_vertices.push_back(this->_vertices[vertex]);
        }

        // make the interfaces for each cell
        std::vector<Grid::Interface *> cell_interfaces;
        std::vector<Grid::Vertex *> interface_vertices;

        for ( int i_vertex=0; i_vertex < n_vertices-1; i_vertex++) {
            interface_vertices.assign({cell_vertices[i_vertex], cell_vertices[i_vertex+1]});
            Grid::Interface * interface = this->_add_interface(interface_vertices);
            cell_interfaces.push_back(interface);
        }
        // the last interface wraps around, so we can't use the above pattern
        // instead we just hard code the closing interface
        interface_vertices.assign({cell_vertices[n_vertices-1], cell_vertices[0]});
        Grid::Interface * interface = this->_add_interface(interface_vertices);
        cell_interfaces.push_back(interface);
        this->_cells.push_back(new Grid::Cell(cell_vertices, cell_interfaces, index));
    }

    // read the boundary conditions
    get_next_line(su2_file, line);
    if (line.find("NMARK") != 0){
        throw std::runtime_error("Could not find number of boundaries");
    }
    int n_boundaries = read_integer(line);
    for (int i_boundary = 0; i_boundary < n_boundaries; i_boundary++){
        get_next_line(su2_file, line);
        // first comes the tag
        std::string tag = read_string(line);

        // next comes the number of elements on this boundary
        get_next_line(su2_file, line);
        if (line.find("MARKER_ELEMS") != 0){
            throw std::runtime_error("Could not find the number of elements on boundary");
        }
        int n_elements = read_integer(line);

        // finally, the actual elements
        std::vector<Grid::Interface *> bndry_interfaces = {};
        for (int i_element = 0; i_element < n_elements; i_element++){
            get_next_line(su2_file, line);
            Grid::Element element = read_element(line);
            if (element.shape != Grid::ElementShape::Line){
                throw std::runtime_error("Boundary element should only be a line");
            }
            std::vector<Grid::Vertex *> boundary_vertices = {};
            for (int vertex_indx : element.vertices){
                boundary_vertices.push_back(this->_vertices[vertex_indx]);
            }
            Grid::Interface * interface = this->_find_interface(boundary_vertices);
            if (!interface) {
                for (int vertex_indx : element.vertices) {
                    std::cout << vertex_indx << ", ";
                }
                std::cout << "\n";
                throw std::runtime_error("Could not find the interface on boundary");
            }

            // add the boundary cell and interface to the boundary condition
            interface->mark_on_boundary(tag);
            bndry_interfaces.push_back(interface);
        }
        this->_bcs.insert(
            std::pair<std::string, std::vector<Grid::Interface *>>(tag, bndry_interfaces)
        );
    }
    // All done
    su2_file.close();
}
