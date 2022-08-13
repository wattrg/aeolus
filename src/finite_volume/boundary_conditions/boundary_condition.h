#ifndef __BOUNDARY_CONDITION_
#define __BOUNDARY_CONDITION_

#include "../cell.h"
#include "../interface.h"
#include "ghost_cell_effect.h"
#include <vector>

class GhostCellEffect;
class Interface;
class Cell;

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
    BoundaryCondition(std::string tag);
    BoundaryCondition(std::vector<GhostCellEffect *> pre_recon, std::string tag);

    // apply the pre-reconstruction actions
    void apply_pre_reconstruction();

    void add_interface(Interface * face);
    void add_ghost_cell(Cell * cell);

    const std::string tag() const {return _tag;};

    void add_pre_recon_action(GhostCellEffect *);

protected:
    std::string _tag;
    // keep track of the ghost cells and interfaces
    // for this particular boundary condition
    std::vector<Cell *> _ghost_cells;
    std::vector<Interface*> _interfaces;

    // actions to perform
    std::vector<GhostCellEffect *> _pre_recon_actions;
};


// There are some specific boundaries specified here
class SlipWall : public BoundaryCondition {
public:
    SlipWall(std::string tag);
};

class SupersonicInflow : public BoundaryCondition {
public:
    SupersonicInflow(FlowState fs, std::string tag);
};

class SupersonicOutflow : public BoundaryCondition {
    SupersonicOutflow(std::string tag);
};


#endif
