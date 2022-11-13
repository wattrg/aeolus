#include "boundary_condition.h"
#include "../cell.h"
#include "../interface.h"
#include "ghost_cell_effect.h"
#include "../../util/unused.h"

BoundaryCondition::BoundaryCondition(){};

BoundaryCondition::BoundaryCondition(std::vector<std::shared_ptr<GhostCellEffect>> pre_recon)
    : _pre_recon_actions(pre_recon)
{}

BoundaryCondition::BoundaryCondition(const BoundaryCondition & other)
    : _pre_recon_actions(other._pre_recon_actions)
{
}


void BoundaryCondition::add_interface(Interface & face){
    this->_interfaces.push_back(face.id());
}

//void BoundaryCondition::add_ghost_cell(Cell & cell){
//    this->_ghost_cells.push_back(&cell);
//}

BoundaryCondition::~BoundaryCondition(){
    //for (GhostCellEffect * gce : this->_pre_recon_actions){
    //    if (gce) delete gce;
    //    gce = 0;
    //}
}

void BoundaryCondition::apply_pre_reconstruction(FluidBlock &fb, std::vector<Interface> &interfaces){
    int number_interfaces = this->_interfaces.size();
    for (int iface = 0; iface < number_interfaces; iface++){
        Interface &face = interfaces[this->_interfaces[iface]];        
        int valid_id = face.get_valid_cell();
        int ghost_id = face.get_ghost_cell();
        FlowState valid = fb.get_flow(valid_id);
        FlowState ghost = fb.get_flow(ghost_id);
        for (std::shared_ptr<GhostCellEffect> action : this->_pre_recon_actions){
            action->apply(face, valid, ghost); 
        }
        fb.set_flow(ghost_id, ghost);
    }
}

// specific implementation of some common boundary conditions
// this should only be a matter of constructing the boundary condition
SlipWall::SlipWall() {
    this->_pre_recon_actions.push_back(
        std::shared_ptr<GhostCellEffect>(new InternalCopy())
    );
    this->_pre_recon_actions.push_back(
        std::shared_ptr<GhostCellEffect>(new ReflectNormal())
    );
}

SupersonicOutflow::SupersonicOutflow() {
    this->_pre_recon_actions.push_back(
        std::shared_ptr<GhostCellEffect>(new InternalCopy())
    );
}

SupersonicInflow::SupersonicInflow(FlowState fs) {
    this->_pre_recon_actions.push_back(
        std::shared_ptr<GhostCellEffect>(new FlowStateCopy(fs))
    ); 
}
