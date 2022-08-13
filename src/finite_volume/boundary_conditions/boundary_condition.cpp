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
        // since multiple boundaries (with different tags) can
        // use the same boundary condition object we have to be
        // careful not to delete it twice
        if (gce) delete gce;
        gce = nullptr;
    }
}

void BoundaryCondition::add_pre_recon_action(GhostCellEffect * gce){
    this->_pre_recon_actions.push_back(gce); 
}


void BoundaryCondition::apply_pre_reconstruction(){
    for (Cell * ghost_cell : this->_ghost_cells){
        for (GhostCellEffect * action : this->_pre_recon_actions){
            action->apply(*ghost_cell);
        }
    }
}

// specific implementation of some common boundary conditions
// this should only be a matter of constructing the boundary condition
SlipWall::SlipWall(std::string tag) : BoundaryCondition(tag) {
    this->_pre_recon_actions.push_back(new InternalCopy());
    this->_pre_recon_actions.push_back(new ReflectNormal());
}

SupersonicOutflow::SupersonicOutflow(std::string tag) : BoundaryCondition(tag) {
    this->_pre_recon_actions.push_back(new InternalCopy());
}

SupersonicInflow::SupersonicInflow(FlowState fs, std::string tag) : BoundaryCondition(tag) {
    this->_pre_recon_actions.push_back(new FlowStateCopy(fs)); 
}
