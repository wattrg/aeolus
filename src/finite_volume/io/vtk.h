#ifndef __VTK_H_
#define __VTK_H_

#include "fluid_block_io.h"
#include <iostream>
#include <fstream>


class VTKWriter : public FluidBlockWriter {
public:
    ~VTKWriter();

    void write_fluid_block(const char & file_name, const FluidBlock & fb);

private:
    std::vector<double> _points;
    std::vector<int> _connectivity;
    std::vector<int> _offsets;
    std::vector<int> _types;
    const std::vector<Cell *> * _cells;
    unsigned int _number_points;
    unsigned int _number_cells;

    // read all the data
    void _read_data(const FluidBlock & fb);

    // write a data array
    template <typename T> void _write_data_array(GridData<T> data);
};




class VTKReader : public FluidBlockReader {
public:
    ~VTKReader();
    void read_fluid_block(const char & file_name, FluidBlock & fb);
};


// some useful functions for VTK files
int cell_shape_to_vtk_type(CellShape::CellShape shape);
#endif
