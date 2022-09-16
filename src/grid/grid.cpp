#include "grid.h"

Grid::GridFormats Grid::extension_to_grid_format(std::string extension){
    if (extension == "su2") return GridFormats::su2;
    throw std::runtime_error("Unknown grid format");
}

Grid::Grid::Grid (std::string file_name){
    // the file extension
    size_t ext_start = file_name.find_last_of('.');
    std::string extension = file_name.substr(ext_start+1);
    GridFormats grid_format = extension_to_grid_format(extension);

    GridInput * grid_reader = nullptr;
    switch (grid_format){
        case GridFormats::su2:
            grid_reader = new Su2GridInput();
            break;
        default:
            throw std::runtime_error("Unknown grid format");
    }

    grid_reader->read_grid(file_name, *this);
}
