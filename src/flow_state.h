#ifndef __FLOW_STATE_H_
#define __FLOW_STATE_H_

#include "gas_state.h"
#include "vector.h"

class FlowState {
public:
    FlowState();
    FlowState(GasState gs, Vector3 vel);
    ~FlowState();
    GasState gas_state;
    Vector3 velocity;
    std::string to_string() const;
    friend std::ostream& operator << (std::ostream& os, const FlowState gs);
};


#endif // __FLOW_STATE_H_
