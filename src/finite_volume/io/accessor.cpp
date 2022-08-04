#include "accessor.h"

std::vector<double> access_pressure(const Cell & cell){
    return std::vector<double> {cell.fs.gas_state.p}; 
}

std::vector<double> access_density(const Cell & cell){
    return std::vector<double> {cell.fs.gas_state.rho};
}

std::vector<double> access_temperature(const Cell & cell){
    return std::vector<double> {cell.fs.gas_state.T};
}

std::vector<double> access_velocity(const Cell & cell){
    Vector3 vel = cell.fs.velocity;
    return std::vector<double> {vel.x, vel.y, vel.z};
}

std::vector<double> Accessor::operator() (const FluidBlock & fb) {
    std::vector<double> data = {};
    for (const Cell * cell : fb.cells()){
        std::vector<double> cell_data = this->_access_from_cell(*cell);
        if (cell_data.size() != this->_number_of_components){
            throw std::runtime_error("Found incorrect number of components");
        }
        for (double component : cell_data){
            data.push_back(component);
        }
    }
    return data;
}
