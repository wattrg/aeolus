#include "gas_model.h"

GasModel::GasModel(double R) : _R(R) {
    _Cv = 3.0 / 2.0 * _R;
    _Cp = 5.0 / 2.0 * _R;
}

void GasModel::update_from_pT(GasState & gas_state) {
    gas_state.rho = gas_state.p / (_R * gas_state.T);
    gas_state.u = _Cv * gas_state.T;
}

void GasModel::update_from_rhoT(GasState & gas_state) {
    gas_state.p = gas_state.rho * _R * gas_state.T;
    gas_state.u = _Cv * gas_state.T;
}

void GasModel::update_from_prho(GasState & gas_state) {
    gas_state.T = gas_state.p / ( gas_state.rho * _R );
    gas_state.u = _Cv * gas_state.T;
}
