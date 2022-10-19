#include "gas_model.h"
#include <cmath>


GasModel::GasModel(double R) : _R(R) {
    // assume that it is air
    _Cv = 1.0/0.4 * _R;
    _Cp = _Cv + _R;
    _gamma = _Cp / _Cv;
}

GasModel::GasModel() {
    // assume air
    GasModel(287.0);
}

void GasModel::_update_sound_speed(GasState & gas_state) {
   gas_state.a = sqrt(this->_gamma * _R * gas_state.T);
}

void GasModel::update_from_pT(GasState & gas_state) {
    gas_state.rho = gas_state.p / (_R * gas_state.T);
    if (gas_state.rho < 0.0) 
        throw GasModelException("update_from_pT failed", &gas_state);
    gas_state.u = _Cv * gas_state.T;
    _update_sound_speed(gas_state);
}

void GasModel::update_from_rhoT(GasState & gas_state) {
    gas_state.p = gas_state.rho * _R * gas_state.T;
    gas_state.u = _Cv * gas_state.T;
    _update_sound_speed(gas_state);
}

void GasModel::update_from_prho(GasState & gas_state) {
    gas_state.T = gas_state.p / ( gas_state.rho * _R );
    gas_state.u = _Cv * gas_state.T;
    _update_sound_speed(gas_state);
}

#ifdef GPU
#pragma omp declare target
#endif
void GasModel::update_from_rhou(GasState & gas_state){
    gas_state.T = gas_state.u / _Cv;
    gas_state.p = gas_state.rho * _R * gas_state.T;
    if (gas_state.T < 0.0){
        //throw GasModelException("update_from_rhou failed", &gas_state);
    }
    _update_sound_speed(gas_state);
}
#ifdef GPU
#pragma omp end declare target
#endif

double GasModel::internal_energy(GasState & gas_state) {
    return gas_state.u;
}

const char * GasModelException::what() const noexcept {
    //std::string msg = std::string(message);
    //if (gs) {
    //    msg += "\n The gas state was:\n";
    //    msg += gs->to_string();
    //}
    return message;
}
