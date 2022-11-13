#include "flow_state.h"

FlowState::FlowState() : gas_state(GasState()), velocity(Vector3()){}

FlowState::FlowState(GasState gs, Vector3 vel) : gas_state(gs), velocity(vel) {}
FlowState::~FlowState(){}

#ifdef GPU
#pragma omp declare target
#endif
void FlowState::encode_conserved(GasModel & gas_model, ConservedQuantity &cq){
    UNUSED(gas_model);
    double vx = this->velocity.x;
    double vy = this->velocity.y;
    double vz = this->velocity.z;
    double p = this->gas_state.p;
    double rho = this->gas_state.rho;
    double u = this->gas_state.u;
    double ke = 0.5 * (vx*vx + vy*vy + vz*vz);
    cq.mass = rho;
    cq.momentum.x = rho*vx;
    cq.momentum.y = rho*vy;
    cq.momentum.z = rho*vz;
    cq.energy = (u + ke)*rho + p;
}
#ifdef GPU
#pragma omp end declare target
#endif

#ifdef GPU
#pragma omp declare target
#endif
void FlowState::decode_conserved(GasModel & gas_model, ConservedQuantity &cq){
    double vx = this->velocity.x;
    double vy = this->velocity.y;
    double vz = this->velocity.z;
    double ke = 0.5 * (vx*vx + vy*vy + vz*vz);
    this->gas_state.rho = cq.mass;
    this->velocity.x = cq.momentum.x / cq.mass;
    this->velocity.y = cq.momentum.y / cq.mass;
    double e = cq.energy/this->gas_state.rho;
    this->gas_state.u = e - ke;
    gas_model.update_from_rhou(this->gas_state);
}
#ifdef GPU
#pragma omp end declare target
#endif

#ifdef GPU
#pragma omp declare target
#endif
void FlowState::copy(const FlowState & other){
    this->gas_state.copy(other.gas_state);
    this->velocity.copy(other.velocity);
}
#ifdef GPU
#pragma omp end declare target
#endif

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
