#include "flux_calc.h"

namespace FluxCalculator{

void roe(FlowState &left, FlowState &right, ConservedQuantity &flux){
    return;
}

void hanel(FlowState &left, FlowState &right, ConservedQuantity &flux){
    return;
}

void zero_flux(FlowState & left, FlowState & right, ConservedQuantity & flux){
    for (double & u : flux.conserved_quantities){
        u = 0.0;
    }
}

}
