#include "gas_state.h"
#include "../util/unused.h"

GasState::GasState() : p(0.0), T(0.0), rho(0.0), u(0.0) {
}

GasState::GasState(GasModel * gm) : p(0.0), T(0.0), rho(0.0), u(0.0) {
    UNUSED(gm);
}


void GasState::copy(const GasState & other){
    this->p = other.p;
    this->T = other.T;
    this->rho = other.rho;
    this->u = other.u;
    this->a = other.a;
}

std::string GasState::to_string() const{
    std::string str = "GasState(";
    str.append("p = ");
    str.append(std::to_string(this->p));
    str.append(" Pa,");
    str.append(" rho = ");
    str.append(std::to_string(this->rho));
    str.append(" kg/m^),");
    str.append(" T = ");
    str.append(std::to_string(this->T));
    str.append(" K,");
    str.append(" u = ");
    str.append(std::to_string(this->u));
    str.append(" J");
    str.append(")");
    return str;
}

std::ostream& operator << (std::ostream& os, const GasState gs){
    os << gs.to_string();
    return os;
}
