#ifndef __GHOST_CELL_EFFECT_
#define __GHOST_CELL_EFFECT_

#include "../cell.h"

class Cell;

class GhostCellEffect {
public:
    virtual ~GhostCellEffect() {};
    virtual void apply(Interface &interface, Cell &valid, Cell &ghost)=0;
};

class FlowStateCopy : public GhostCellEffect{
public:
    ~FlowStateCopy() {};
    FlowStateCopy(const FlowState fs) : _fs(fs) {};
    FlowStateCopy(const FlowStateCopy & other);
    void apply(Interface &interface, Cell &valid, Cell &ghost);

private:
    const FlowState _fs;
};

class InternalCopy : public GhostCellEffect{
public:
    ~InternalCopy() {};
    InternalCopy() {};
    InternalCopy(const InternalCopy & other);
    void apply(Interface &interface, Cell &valid, Cell &ghost);
};

class ReflectNormal : public GhostCellEffect {
public:
    ~ReflectNormal() {};
    ReflectNormal() {};
    ReflectNormal(const ReflectNormal & other);
    void apply(Interface &interface, Cell &valid, Cell &ghost);
};

//class ReflectTangential : public GhostCellEffect {
//public:
//    ~ReflectTangential(){};
//    ReflectTangential() {};
//    ReflectTangential(const ReflectTangential & other);
//    void apply(Cell & ghost_cell) {}
//};

#endif
