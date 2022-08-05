#include "vtk.h"


// VTK writer
VTKWriter::~VTKWriter() {}

void VTKWriter::write_fluid_block(const char & file_name, const FluidBlock & fb){
    std::fstream vtk_file;
    vtk_file.open(&file_name);
    vtk_file << "<VTKFile type='UnstructuredGrid' byte_order='BigEndian'>\n";
    vtk_file << "<UnstructuredGrid>\n"; 
    vtk_file << "<Piece NumberOfPoints='" << this->_number_points << "' ";
    vtk_file << "NumberOfCells='" << this->_number_cells << "'>\n";
    vtk_file << "<Points>\n";

    vtk_file << "</Points>\n";
    vtk_file << "</Piece>\n";
    vtk_file << "</UnstructuredGrid>\n";
    vtk_file << "</VTKFile>";
}


void VTKWriter::_read_data(const FluidBlock & fb){
    this->_cells = &fb.cells();
    this->_number_cells = this->_cells->size();
    this->_number_points = 0;

    // loop over all the cells in the fluid block, storing the required data in
    // the VTKWriter
    for (const Cell * cell : *this->_cells){
        // get data about the geometry
        this->_number_points += cell->number_vertices(); 
        for (const Vertex * vertex : cell->vertices()){
            Vector3 pos = vertex->get_pos();
            this->_points.push_back(pos.x);
            this->_points.push_back(pos.y);
            this->_points.push_back(pos.z);
            this->_connectivity.push_back(vertex->id());
        }
        this->_offsets.push_back(cell->number_vertices());
        this->_types.push_back(cell_shape_to_vtk_type(cell->get_shape())); 

        // read the flow variables via the data accessors
        for (unsigned int var_i = 0; var_i < _variable_accessors.size(); var_i++){
            std::vector<double> data = _variable_accessors[var_i]->read_variable(*cell);
            for (double d : data){
                _flow_data[var_i].data.push_back(d);
            }
        }
    }
}




// VTK reader 
VTKReader::~VTKReader() {}

void VTKReader::read_fluid_block(const char & file_name, FluidBlock & fb){
    std::runtime_error("Reading vtk files not implemented yet");
}


// convert CellShape to VTK cell type
int cell_shape_to_vtk_type (CellShape::CellShape shape){
    switch (shape){
        case CellShape::Quad:
            return 9;
        default:
            throw std::runtime_error("Unknown CellShape");
    }
}
