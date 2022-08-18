#include "boundary_condition.h"

BoundaryCondition::BoundaryCondition(){};

BoundaryCondition::BoundaryCondition(std::vector<std::shared_ptr<GhostCellEffect>> pre_recon, std::string tag)
    : _tag(tag), _pre_recon_actions(pre_recon)
{}


void BoundaryCondition::add_interface(Interface * face){
    this->_interfaces.push_back(face);
}

void BoundaryCondition::add_ghost_cell(Cell * cell){
    this->_ghost_cells.push_back(cell);
}

BoundaryCondition::~BoundaryCondition(){
    //for (GhostCellEffect * gce : this->_pre_recon_actions){
    //    if (gce) delete gce;
    //    gce = 0;
    //}
}

void BoundaryCondition::apply_pre_reconstruction(){
    for (Cell * ghost_cell : this->_ghost_cells){
        for (std::shared_ptr<GhostCellEffect> action : this->_pre_recon_actions){
            action->apply(*ghost_cell);
        }
    }
}

// specific implementation of some common boundary conditions
// this should only be a matter of constructing the boundary condition
SlipWall::SlipWall() {
    this->_pre_recon_actions.push_back(std::shared_ptr<GhostCellEffect>(new InternalCopy()));
    this->_pre_recon_actions.push_back(std::shared_ptr<GhostCellEffect>(new ReflectNormal()));
}

SupersonicOutflow::SupersonicOutflow() {
    this->_pre_recon_actions.push_back(std::shared_ptr<GhostCellEffect>(new InternalCopy()));
}

SupersonicInflow::SupersonicInflow(FlowState fs) {
    this->_pre_recon_actions.push_back(std::shared_ptr<GhostCellEffect>(new FlowStateCopy(fs))); 
}
