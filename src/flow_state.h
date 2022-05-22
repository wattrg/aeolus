#ifndef __FLOW_STATE_H_
#define __FLOW_STATE_H_

#include "gas_state.h"
#include "vector.h"

class FlowState {
public:
    FlowState();
    ~FlowState();
    GasState gas_state;
    Vector2 velocity;
    friend std::ostream& operator << (std::ostream& os, const FlowState gs);
};


#endif // __FLOW_STATE_H_
