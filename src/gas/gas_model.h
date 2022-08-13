#ifndef __GAS_MODEL_H_
#define __GAS_MODEL_H_

#include "gas_state.h"

class GasState;

class GasModel {
public:
    GasModel();
    GasModel(double R);
    void update_from_pT(GasState & gas_state);
    void update_from_rhoT(GasState & gas_state);
    void update_from_prho(GasState & gas_state);
    void update_from_rhou(GasState & gas_state);

    double internal_energy(GasState & gas_state);

private:
    double _R;
    double _Cv;
    double _Cp;

    void _update_sound_speed(GasState & gas_state);
};


#endif // __GAS_MODEL_H_
