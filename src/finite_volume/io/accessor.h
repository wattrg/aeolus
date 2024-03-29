#ifndef __ACCESSOR_H_
#define __ACCESSOR_H_

#include <vector>
#include "../fluid_block.h"

class FluidBlock;
class Cell;


// pipeline for accessing data from a cell.
class Accessor {
public: 
    Accessor(std::string name,
             std::vector<double> (*access_from_cell)(const Cell &, const FlowState &), 
             int number_of_components);

    // access a particular variable from a fluid block
    // this returns a vector of a particular type
    std::vector<double> read_variable(const Cell & cell, const FlowState &);

    unsigned int number_of_components();
    std::string name();

protected:
    std::vector<double> (*_access_from_cell)(const Cell &, const FlowState &);

private:
    unsigned int _number_of_components;
    std::string _name;
};

// functions to actually get the data from an individual cell
std::vector<double> access_pressure(const Cell &, const FlowState &);
std::vector<double> access_density(const Cell &, const FlowState &);
std::vector<double> access_temperature(const Cell &, const FlowState &);
std::vector<double> access_velocity(const Cell &, const FlowState &);
std::vector<double> access_energy(const Cell &, const FlowState &);
std::vector<double> access_volume(const Cell &, const FlowState &);
std::vector<double> access_speed_of_sound(const Cell &, const FlowState &);

#endif
