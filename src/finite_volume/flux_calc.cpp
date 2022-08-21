#include "flux_calc.h"

namespace FluxCalculator{

void roe(FlowState &left, FlowState &right, ConservedQuantity &flux){
    return;
}

void hanel(FlowState &left, FlowState &right, ConservedQuantity &flux){
    // unpack left state
    double rL = left.gas_state.rho;
    double pL = left.gas_state.p;
    double pLrL = pL / rL;
    double uL = left.velocity.x;
    double vL = left.velocity.y;
    double wL = left.velocity.z;
    double eL = left.gas_state.u;
    double aL = left.gas_state.a;
    double keL = 0.5 * (uL*uL + vL*vL + wL*wL);
    double HL = eL + pLrL + keL;

    // unpack right state
    double rR = right.gas_state.rho;
    double pR = right.gas_state.p;
    double pRrR = pL / rL;
    double uR = right.velocity.x;
    double vR = right.velocity.y;
    double wR = right.velocity.z;
    double eR = right.gas_state.u;
    double aR = right.gas_state.a;
    double keR = 0.5 * (uR*uR + vR*vR + wR*wR);
    double HR = eR + pRrR + keR;

    // left state pressure and velocity splitting
    double pLplus=0, uLplus=0;
    if (fabs(uL) <= aL){
        uLplus = 1.0/(4.0*aL) * (uL+aL)*(uL+aL);
        pLplus = pL*uLplus * (1.0/aL * (2.0-uL/aL));
    }
    else{
        uLplus = 0.5*(uL+fabs(uL));
        pLplus = pL*uLplus * (1.0/uL);
    }

    // right state pressure and velocity splitting
    double pRminus=0, uRminus=0;
    if (fabs(uR) <= aR){
        uRminus = -1.0/(4.0*aR) * (uR-aR)*(uR-aR);
        pRminus = pR*uRminus * (1.0/aR * (-2.0-uR/aR));
    }
    else{
        uRminus = 0.5*(uR-fabs(uR));
        pRminus = pR*uRminus * (1.0/uR);
    }

    // pressure flux
    double p_half = pLplus + pRminus;

    // assemble components into the flux vector
    flux[flux.rho()] = uLplus * rL + uRminus * rR;
    unsigned int momentum = flux.momentum();
    flux[momentum] = uLplus * rL * uL + uRminus * rR * uR + p_half;
    flux[momentum+1] = uLplus * rL * vL + uRminus * rL * vR;
    if (flux.dimensions() > 2){
        flux[momentum+2] = uLplus * rL * wL + uRminus * rR * wR;
    }
    flux[flux.energy()] = uLplus *rL * HL + uRminus * rR * HR; 

    return;
}

}
