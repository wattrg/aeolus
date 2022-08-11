#include "boundary_condition.h"

BoundaryCondition::BoundaryCondition(std::vector<GhostCellEffect *> pre_recon, std::string tag)
    : _tag(tag), _pre_recon_actions(pre_recon)
{}

BoundaryCondition::BoundaryCondition(std::string tag) : _tag(tag) {}

void BoundaryCondition::add_interface(Interface * face){
    this->_interfaces.push_back(face);
}

void BoundaryCondition::add_ghost_cell(Cell * cell){
    this->_ghost_cells.push_back(cell);
}

BoundaryCondition::~BoundaryCondition(){
    for (GhostCellEffect * gce : this->_pre_recon_actions){
        delete gce;
    }
}

void BoundaryCondition::apply_pre_reconstruction(){
    for (Cell * ghost_cell : this->_ghost_cells){
        for (GhostCellEffect * action : this->_pre_recon_actions){
            action->apply(*ghost_cell);
        }
    }
}

