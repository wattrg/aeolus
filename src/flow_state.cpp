#include "flow_state.h"

FlowState::FlowState(){}
FlowState::~FlowState(){}

std::ostream& operator << (std::ostream& os, const FlowState fs) {
    os << "FlowState(";
    os << "gas_state = " << fs.gas_state << ", ";
    os << "velocity = " << fs.velocity << ")";
    return os;
}
