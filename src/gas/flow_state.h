#ifndef __FLOW_STATE_H_
#define __FLOW_STATE_H_

#include "../gas/gas_state.h"
#include "../util/vector3.h"
#include "../util/unused.h"
#include "../finite_volume/conserved_quantities.h"

class FlowState {
public:
    FlowState();
    FlowState(GasState gs, Vector3 vel);
    ~FlowState();

    GasState gas_state;
    Vector3 velocity;

    
    // encode conserved quantities
    void encode_conserved(GasModel & gas_model, ConservedQuantity &cq);

    // decode consered quantities
    void decode_conserved(GasModel & gas_model, ConservedQuantity &cq);

    void copy(const FlowState & other);


    std::string to_string() const;
    friend std::ostream& operator << (std::ostream& os, const FlowState gs);
};


#endif // __FLOW_STATE_H_
