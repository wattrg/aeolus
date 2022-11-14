#ifndef __GHOST_CELL_EFFECT_
#define __GHOST_CELL_EFFECT_

#include "../cell.h"

class Cell;

class GhostCellEffect {
public:
    virtual ~GhostCellEffect() {};
    virtual void apply(Interface &interface, FlowState &valid, FlowState &ghost)=0;
};

class FlowStateCopy : public GhostCellEffect{
public:
    ~FlowStateCopy() {};
    FlowStateCopy(const FlowState fs) : _fs(fs) {};
    FlowStateCopy(const FlowStateCopy & other);
    void apply(Interface &interface, FlowState &valid, FlowState &ghost);

private:
    const FlowState _fs;
};

class InternalCopy : public GhostCellEffect{
public:
    ~InternalCopy() {};
    InternalCopy() {};
    InternalCopy(const InternalCopy & other);
    void apply(Interface &interface, FlowState &valid, FlowState &ghost);
};

class ReflectNormal : public GhostCellEffect {
public:
    ~ReflectNormal() {};
    ReflectNormal() {};
    ReflectNormal(const ReflectNormal & other);
    void apply(Interface &interface, FlowState &valid, FlowState &ghost);
};

#endif
