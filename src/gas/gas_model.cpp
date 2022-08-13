#include "gas_model.h"
#include <cmath>


GasModel::GasModel(double R) : _R(R) {
    _Cv = 3.0 / 2.0 * _R;
    _Cp = 5.0 / 2.0 * _R;
}

GasModel::GasModel() {
    // assume air
    GasModel(287.0);
}

void GasModel::_update_sound_speed(GasState & gas_state) {
   double cp = _Cv + _R;
   double gamma = cp / _Cv;
   gas_state.a = sqrt(gamma * _R * gas_state.T);
}

void GasModel::update_from_pT(GasState & gas_state) {
    gas_state.rho = gas_state.p / (_R * gas_state.T);
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

void GasModel::update_from_rhou(GasState & gas_state){
    gas_state.T = gas_state.u / _Cv;
    gas_state.p = gas_state.rho * _R * gas_state.T;
    _update_sound_speed(gas_state);
}

double GasModel::internal_energy(GasState & gas_state) {
    return gas_state.u;
}
