#ifndef __FLUX_CALC_H
#define __FLUX_CALC_H
#include "../gas/flow_state.h"
#include "conserved_quantities.h"



enum class FluxCalculators {
    hanel,
    ausmdv,
};


namespace FluxCalculator{
    void hanel(FlowState & left, FlowState & right, ConservedQuantity & flux);
    void ausmdv(FlowState & left, FlowState & right, ConservedQuantity & flux);
}

#endif
