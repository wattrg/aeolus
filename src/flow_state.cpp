#include "flow_state.h"

FlowState::FlowState() : gas_state(GasState()), velocity(Vector3()){}

FlowState::FlowState(GasState gs, Vector3 vel) : gas_state(gs), velocity(vel) {}
FlowState::~FlowState(){}

void FlowState::copy(FlowState & other){
    this->gas_state.copy(other.gas_state);
    this->velocity.copy(other.velocity);
}

std::string FlowState::to_string() const {
    std::string str = "FlowState(";
    str.append(this->gas_state.to_string());
    str.append(", ");
    str.append("velocity = ");
    str.append(this->velocity.to_string());
    str.append(")");
    return str;
}

std::ostream& operator << (std::ostream& os, const FlowState fs) {
    os << fs.to_string();
    return os;
}
