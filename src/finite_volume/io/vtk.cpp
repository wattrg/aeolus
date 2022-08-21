#include "vtk.h"


// VTK writer
VTKWriter::~VTKWriter() {
    // on the way out, write a pvd file to so paraview can open 
    // all the time values at once
    std::ofstream pvd_file("flow/flow.pvd");
    pvd_file << "<?xml version='1.0'?>\n";
    pvd_file << "<VTKFile type='Collection' version='0.1' byte_order='LittleEndian'>\n";
    pvd_file << "<Collection>\n";
    for (unsigned int i = 0; i < this->_times.size(); i++){
        pvd_file << "<DataSet timestep='" << this->_times[i] << "' group='' part='0' ";
        pvd_file << "file='" << this->_block_names[i] << "'/>\n";
    }
    pvd_file << "</Collection>\n";
    pvd_file << "</VTKFile>\n";
}

VTKWriter::VTKWriter():
    _points(GridData<double> ("points", 3, "Float32")),
    _connectivity(GridData<int> ("connectivity", 1, "Int32")),
    _offsets(GridData<int> ("offsets", 1, "Int32")),
    _types(GridData<int> ("types", 1, "Int32")),
    _number_points(0),
    _number_cells(0)
{}

void VTKWriter::write_fluid_block(const char & file_name, const FluidBlock & fb, double time){
    this->_read_data(fb);
    std::string f_name = std::string("flow/") + std::string(&file_name);
    std::ofstream vtk_file(f_name);
    vtk_file << "<VTKFile type='UnstructuredGrid' byte_order='BigEndian'>\n";
    vtk_file << "<UnstructuredGrid>\n"; 
    vtk_file << "<Piece NumberOfPoints='" << this->_number_points << "' ";
    vtk_file << "NumberOfCells='" << this->_number_cells << "'>\n";
    vtk_file << "<Points>\n";
    this->_write_data_array(this->_points, vtk_file);
    vtk_file << "</Points>\n";
    vtk_file << "<Cells>\n";
    this->_write_data_array(this->_connectivity, vtk_file);
    this->_write_data_array(this->_offsets, vtk_file);
    this->_write_data_array(this->_types, vtk_file);
    vtk_file << "</Cells>\n";
    vtk_file << "<CellData>\n";
    for (GridData<double> data : this->_flow_data){
        this->_write_data_array(data, vtk_file);
    }
    vtk_file << "</CellData>\n";
    vtk_file << "</Piece>\n";
    vtk_file << "</UnstructuredGrid>\n";
    vtk_file << "</VTKFile>\n";
    vtk_file.close();
    this->_clear_data();

    this->_times.push_back(time);
    this->_block_names.push_back(std::string(&file_name));
}

void VTKWriter::_clear_data(){
    this->_points.clear();
    this->_connectivity.clear();
    this->_offsets.clear();
    this->_types.clear();
    this->_cells = nullptr;
    this->_flow_data.clear();
    _number_points = 0;
    _number_cells = 0;
}

void VTKWriter::_read_data(const FluidBlock & fb){
    this->_cells = &fb.cells();
    this->_number_cells = this->_cells->size();


    // loop over all the vertices in the fluid block, storing the required data in
    // the VTKWriter
    const std::vector<Vertex *> vertices = fb.vertices();
    this->_number_points = vertices.size();
    for (const Vertex * vertex : vertices){
        Vector3 pos = vertex->get_pos();
        this->_points.data.push_back(pos.x);
        this->_points.data.push_back(pos.y);
        this->_points.data.push_back(pos.z);
    }

    // set up the storage for all the flow variables
    for (unsigned int var_i = 0; var_i < _variable_accessors.size(); var_i++){
        Accessor & var_access = _variable_accessors[var_i];
        GridData<double> variable = GridData<double>(var_access.name(), 
                                                     var_access.number_of_components(), 
                                                     "Float32");
        _flow_data.push_back(variable);
    }

    // gather data from each cell
    int offset = 0;
    for (const Cell * cell : *this->_cells){
        // get data about the geometry
        offset += cell->number_vertices();
        this->_offsets.data.push_back(offset);
        this->_types.data.push_back(cell_shape_to_vtk_type(cell->get_shape())); 
        for (const Vertex * vertex : cell->vertices()){
            this->_connectivity.data.push_back(vertex->id()); 
        }

        // read the flow variables via the data accessors
        for (unsigned int var_i = 0; var_i < _variable_accessors.size(); var_i++){
            Accessor & var_access = _variable_accessors[var_i];
            std::vector<double> data = var_access.read_variable(*cell);
            if (data.size() != var_access.number_of_components()){
                throw std::runtime_error("Incorrect number of components for flow variable");
            }
            for (double d : data){
                _flow_data[var_i].data.push_back(d);
            }
        }
    }
}

template <typename T>
void VTKWriter::_write_data_array(const GridData<T> & data, std::ofstream & vtk_file){
    vtk_file << "<DataArray type='" << data.type_name << "'";
    if (data.number_components != 0){
        vtk_file << " NumberOfComponents='" << data.number_components << "'";
    }
    vtk_file << " Name='" << data.name << "'";
    vtk_file << " format='ascii'>\n";
    for (unsigned int i = 0; i < data.data.size(); i+=data.number_components){
        for (unsigned int i_comp = 0; i_comp < data.number_components-1; i_comp++){
            vtk_file << data.data[i+i_comp] << " ";
        }
        vtk_file << data.data[i+data.number_components-1] << "\n";
    }
    vtk_file << "</DataArray>\n";
}


// VTK reader 
VTKReader::~VTKReader() {}

void VTKReader::read_fluid_block(const char & file_name, FluidBlock & fb){
    std::runtime_error("Reading vtk files not implemented yet");
}


// convert CellShape to VTK cell type
int cell_shape_to_vtk_type (CellShape::CellShape shape){
    switch (shape){
        case CellShape::Triangle:
            return 5;
        case CellShape::Quad:
            return 9;
        default:
            throw std::runtime_error("Unknown CellShape");
    }
}
