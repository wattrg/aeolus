#ifndef __VTK_H_
#define __VTK_H_

#include "fluid_block_io.h"
#include <iostream>
#include <fstream>


class VTKWriter : public FluidBlockWriter {
public:
    ~VTKWriter();
    VTKWriter();
    void write_fluid_block(const char & file_name, const FluidBlock & fb, double time);

private:
    GridData<double> _points;
    GridData<int> _connectivity;
    GridData<int> _offsets;
    GridData<int> _types;
    const std::vector<Cell *> * _cells;
    unsigned int _number_points;
    unsigned int _number_cells;

    // read all the data
    void _read_data(const FluidBlock & fb);

    // clear the data
    void _clear_data();

    // write a data array
    template <typename T> void _write_data_array(const GridData<T> & data, std::ofstream & vtk_file);

    // time values
    std::vector<double> _times;
    std::vector<std::string> _block_names;
};





// some useful functions for VTK files
int cell_shape_to_vtk_type(Grid::CellShape shape);
#endif
