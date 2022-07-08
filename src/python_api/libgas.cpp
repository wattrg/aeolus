#include "../gas_state.h"

extern "C" {
    GasState * GasState_new() { return new GasState(); } 
    double GasState_get_pressure(GasState * gs) {return gs->p;}
    void GasState_set_pressure(GasState * gs, double p) { gs->p = p; }
}
