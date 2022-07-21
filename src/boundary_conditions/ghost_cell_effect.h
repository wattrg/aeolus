#ifndef __GHOST_CELL_EFFECT_
#define __GHOST_CELL_EFFECT_

#include "../cell.h"

class GhostCellEffect {
public:
    virtual void apply(Cell & ghost_cell)=0;
};

class FlowStateCopy : public GhostCellEffect{
public:
    FlowStateCopy(FlowState & fs);
    void apply(Cell & ghost_cell);

private:
    FlowState & _fs;
};

class InternalCopy : public GhostCellEffect{
public:
    InternalCopy();
    void apply(Cell & ghost_cell);
};

class ReflectNormal : public GhostCellEffect {
public:
    ReflectNormal();
    void apply(Cell & ghost_cell);
};

class ReflectTangential : public GhostCellEffect {
public:
    ReflectTangential();
    void apply(Cell & ghost_cell) {}
};

#endif
