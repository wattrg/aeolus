#include "interface.h"
#include "cell.h"
#include "config.h"

#include <stdio.h>

Interface::Interface(Grid::Interface & grid_face)
    : _norm(grid_face.norm()), 
      _tan1(grid_face.tan1()), 
      _tan2(grid_face.tan2()), 
      _area(grid_face.area()),
      _id(grid_face.id())
{
    // the id's of the cells that will be attached
    this->_left_cell_id = grid_face.get_left_cell_id();
    this->_right_cell_id = grid_face.get_right_cell_id();

    if (this->_left_cell_id >= 0) this->_left_cell_valid = true;
    if (this->_right_cell_id >= 0) this->_right_cell_valid = true;

    // the id's of the vertices
    std::vector<Grid::Vertex *> grid_vertices = grid_face.vertices();
    this->_number_vertices = grid_vertices.size();
    for (int i = 0; i < this->_number_vertices; i ++){
        this->_vertices[i] = grid_vertices[i]->id();
    }

    // set up the storage for the flux
    this->_dim = (this->_number_vertices == 2) ? 2 : 3;
    this->_flux = ConservedQuantity();
}


void Interface::copy_left_flow_state(FlowState & fs){
    this->_left.copy(fs);
}

void Interface::copy_right_flow_state(FlowState & fs){
    this->_right.copy(fs);
}

void Interface::copy_left_flow_state(double p, double T, double rho, 
                                     double u, double a, 
                                     double vx, double vy, double vz)
{
    this->_left.gas_state.p = p;
    this->_left.gas_state.T = T;
    this->_left.gas_state.rho = rho;
    this->_left.gas_state.u = u;
    this->_left.gas_state.a = a;
    this->_left.velocity.x = vx;
    this->_left.velocity.y = vy;
    this->_left.velocity.z = vz;
}

void Interface::copy_right_flow_state(double p, double T, double rho, 
                                      double u, double a, 
                                      double vx, double vy, double vz)
{
    this->_right.gas_state.p = p;
    this->_right.gas_state.T = T;
    this->_right.gas_state.rho = rho;
    this->_right.gas_state.u = u;
    this->_right.gas_state.a = a;
    this->_right.velocity.x = vx;
    this->_right.velocity.y = vy;
    this->_right.velocity.z = vz;
}

bool Interface::is_on_boundary() const { return this->_is_on_boundary; }

double Interface::area() const {
    return _area;
}

ConservedQuantity & Interface::flux() {
    return _flux;
}

Interface::~Interface(){}

void Interface::compute_flux(flux_calculator flux_calc){
    this->_transform_flowstate_to_local_frame();
    FluxCalculator::hanel(this->_left, this->_right, this->_flux);
    this->_transform_flux_to_global_frame();
}
void Interface::_transform_flux_to_global_frame(){
    this->_flux.momentum.transform_to_global_frame(this->_norm, this->_tan1, this->_tan2);
}

void Interface::_transform_flowstate_to_local_frame(){

    this->_left.velocity.transform_to_local_frame(this->_norm, this->_tan1, this->_tan2);
    this->_right.velocity.transform_to_local_frame(this->_norm, this->_tan1, this->_tan2);
}
void Interface::_transform_flowstate_to_global_frame(){
    this->_left.velocity
               .transform_to_global_frame(this->_norm, this->_tan1, this->_tan2);
    this->_right.velocity
                .transform_to_global_frame(this->_norm, this->_tan1, this->_tan2);
}

int Interface::get_valid_cell(){
    bool left = this->_left_cell_valid;
    bool right = this->_right_cell_valid;

    if (left && right) throw std::runtime_error("Both cells are valid");
    if (!left && !right) throw std::runtime_error("Neither cell is valid");
    return (left) ? this->_left_cell_id : this->_right_cell_id;
}

int Interface::get_ghost_cell(){
    bool left = this->_left_cell_valid;
    bool right= this->_right_cell_valid;

    if (left && right) throw std::runtime_error("Neight cell is a ghost cell");
    if (!left && !right) throw std::runtime_error("Both cells are ghost cells");
    return (left) ? this->_right_cell_id : this->_left_cell_id;
}

std::ostream& operator << (std::ostream& os, const Interface interface){
    os << "Interface(";
    os << "vertices = [";
    for (int vertex_id : interface._vertices){
        os << vertex_id << ", ";
    }
    os << "], ";
    os << "left = " << interface._left_cell_id << ", ";
    os << "right = " << interface._right_cell_id << ", ";
    os << "on boundary = " << interface._is_on_boundary << ", ";
    os << ")";

    return os;
}

int Interface::get_left_cell() {
    return this->_left_cell_id;
}

int Interface::get_right_cell() {
    return this->_right_cell_id;
}

void Interface::attach_cell_left(Cell & cell){
    if (this->_left_cell_id < 0){
        this->_left_cell_id = cell.id();
        this->_left_cell_valid = cell.is_valid();
        return;
    }
    std::string msg = "There is already a cell to the left. The cell's ID is: ";
    msg += std::to_string(this->_left_cell_id);
    throw std::runtime_error(msg);
}

void Interface::attach_cell_right(Cell & cell){
    if (this->_right_cell_id < 0){
        this->_right_cell_id = cell.id();
        this->_right_cell_valid = cell.is_valid();
        return;
    }
    std::string msg = "There is already a cell to the right. The cell's ID is: ";
    msg += std::to_string(this->_right_cell_id);
    throw std::runtime_error(msg);
    
}

void Interface::mark_on_boundary() {
    this->_is_on_boundary = true;
    // this->_boundary_tag = tag;
}

