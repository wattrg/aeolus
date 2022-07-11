#ifndef __FLUX_CALC_H
#define __FLUX_CALC_H
#include "flow_state.h"
#include "conserved_quantities.h"


namespace FluxCalculators {

enum FluxCalculators {
    roe,
    hanel,
};

}

namespace FluxCalculator{
    void roe(FlowState & left, FlowState & right, ConservedQuantity & flux);
    void hanel(FlowState & left, FlowState & right, ConservedQuantity & flux);
    void zero_flux(FlowState &left, FlowState & right, ConservedQuantity & flux);
}

#endif
