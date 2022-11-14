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

    #ifdef GPU
    #pragma omp declare target
    #endif
    double gamma() const {return _gamma;}
    double Cv() const {return _Cv;}
    double R() const {return _R;}
    #ifdef GPU
    #pragma omp end declare target
    #endif

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
    GasModelException(const char * msg) : message(msg) {}
    GasModelException(const char *  msg, GasState * gs) 
        : gs(gs), message(msg)
    {}
    const char * what() const noexcept override;
    GasState * gs=nullptr;
    const char * message;
};

#endif // __GAS_MODEL_H_
