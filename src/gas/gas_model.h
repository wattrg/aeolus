#ifndef __GAS_MODEL_H_
#define __GAS_MODEL_H_

#include "gas_state.h"
#include <cstring>

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
    double _gamma;

    void _update_sound_speed(GasState & gas_state);
};

// For exceptions relating to the gas model
class GasModelException : public std::exception {
public:
    GasModelException(std::string msg) : message(msg) {}
    GasModelException(std::string msg, GasState * gs) 
        : message(msg), gs(gs)
    {}
    virtual char const * what();

private:
    std::string message;
    GasState * gs=nullptr;
};

#endif // __GAS_MODEL_H_
