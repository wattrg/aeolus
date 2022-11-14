#include "accessor.h"

std::vector<double> access_pressure(const Cell &, const FlowState &fs){
    return std::vector<double> {fs.gas_state.p}; 
}

std::vector<double> access_density(const Cell &, const FlowState &fs){
    return std::vector<double> {fs.gas_state.rho};
}

std::vector<double> access_temperature(const Cell &, const FlowState &fs){
    return std::vector<double> {fs.gas_state.T};
}

std::vector<double> access_velocity(const Cell &, const FlowState &fs){
    Vector3 vel = fs.velocity;
    return std::vector<double> {vel.x, vel.y, vel.z};
}

std::vector<double> access_energy(const Cell &, const FlowState &fs){
    return std::vector<double> {fs.gas_state.u};
}

std::vector<double> access_volume(const Cell &cell, const FlowState &){
    return std::vector<double> {cell.volume()};
}

std::vector<double> access_speed_of_sound(const Cell &, const FlowState &fs){
    return std::vector<double> {fs.gas_state.a};
}

Accessor::Accessor(std::string name,
                   std::vector<double> (*access_from_cell)(const Cell &, const FlowState &), 
                   int number_of_components):
    _access_from_cell(access_from_cell),
    _number_of_components(number_of_components), 
    _name(name)
{}

unsigned int Accessor::number_of_components(){ return _number_of_components; }
std::string Accessor::name() { return _name; }

std::vector<double> Accessor::read_variable (const Cell &cell, const FlowState &fs) {
    std::vector<double> data = {};
    std::vector<double> cell_data = this->_access_from_cell(cell, fs);
    if (cell_data.size() != this->_number_of_components){
        throw std::runtime_error("Found incorrect number of components");
    }
    for (double component : cell_data){
        data.push_back(component);
    }
    return data;
}
