#ifndef __GHOST_CELL_EFFECT_
#define __GHOST_CELL_EFFECT_

#include "../cell.h"

class GhostCellEffect {
public:
    virtual void apply(Cell & ghost_cell)=0;
};

class FixedPT : public GhostCellEffect{
public:
    FixedPT(double p, double T);
    void apply(Cell & ghost_cell);

private:
    double _p, _T;
};

class ExtrapolateCopy : public GhostCellEffect{
public:
    ExtrapolateCopy();
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
