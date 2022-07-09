#include "gas_state.h"

GasState::GasState() : p(0.0), T(0.0), rho(0.0), u(0.0), _gm(new GasModel()) {}

GasState::GasState(GasModel gm) : p(0.0), T(0.0), rho(0.0), u(0.0), _gm(&gm) {}

GasState::~GasState() {
    delete _gm;
}

void GasState::update_from_pT() { this->_gm->update_from_pT(*this); }
void GasState::update_from_prho() { this->_gm->update_from_prho(*this); }
void GasState::update_from_rhoT() { this->_gm->update_from_rhoT(*this); }

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
