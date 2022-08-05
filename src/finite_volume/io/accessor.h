#ifndef __ACCESSOR_H_
#define __ACCESSOR_H_

#include <vector>
#include "../fluid_block.h"

class FluidBlock;


// pipeline for accessing data from a cell.
class Accessor {
public: 
    Accessor(std::string name,
             std::vector<double> (*access_from_cell)(const Cell &), 
             int number_of_components);

    // access a particular variable from a fluid block
    // this returns a vector of a particular type
    std::vector<double> read_variable(const Cell & cell);

    unsigned int number_of_components();

protected:
    std::vector<double> (*_access_from_cell)(const Cell &);

private:
    unsigned int _number_of_components;
};

// functions to actually get the data from an individual cell
std::vector<double> access_pressure(const Cell &);
std::vector<double> access_density(const Cell &);
std::vector<double> access_temperature(const Cell &);
std::vector<double> access_velocity(const Cell &);

#endif
