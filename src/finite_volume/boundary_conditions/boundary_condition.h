#ifndef __BOUNDARY_CONDITION_
#define __BOUNDARY_CONDITION_

//#include "../cell.h"
//#include "../interface.h"
//#include "ghost_cell_effect.h"
#include <vector>
#include <memory>
#include <string>
#include "../../gas/flow_state.h"

class GhostCellEffect;
class Interface;
class Cell;
class FluidBlock;

// we'll keep track of the available boundary conditions here
namespace BoundaryConditions {
enum BcType {
    SlipWall,
    SupersonicInflow,
    SupersonicOutflow,
    UserDefined,
};
}


// This is general enough to represent all boundary conditions
class BoundaryCondition{
public:
    ~BoundaryCondition();
    BoundaryCondition();
    BoundaryCondition(const BoundaryCondition &);
    BoundaryCondition(std::vector<std::shared_ptr<GhostCellEffect>> pre_recon);

    // apply the pre-reconstruction actions
    void apply_pre_reconstruction(FluidBlock &fb, std::vector<Interface> &interfaces);

    void add_interface(Interface & face);
    //void add_ghost_cell(Cell & cell);

    // const std::string tag() const {return _tag;};


protected:
    // the interfaces along the boundary
    std::vector<int> _interfaces;

    // actions to perform
    std::vector<std::shared_ptr<GhostCellEffect>> _pre_recon_actions;
};


// There are some specific boundaries specified here
class SlipWall : public BoundaryCondition {
public:
    SlipWall();
};

class SupersonicInflow : public BoundaryCondition {
public:
    SupersonicInflow(FlowState fs);
};

class SupersonicOutflow : public BoundaryCondition {
public:
    SupersonicOutflow();
};


#endif
