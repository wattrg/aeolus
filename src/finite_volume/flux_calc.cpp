#include "flux_calc.h"

namespace FluxCalculator{

#ifdef GPU
#pragma omp declare target
#endif
void ausmdv(FlowState & left, FlowState & right, ConservedQuantity & flux){
    double rL = left.gas_state.rho;
    double pL = left.gas_state.p;
    double pLrL = pL / rL;
    double uL = left.velocity.x;
    double vL = left.velocity.y;
    double wL = left.velocity.z;
    double eL = left.gas_state.u;
    double aL = left.gas_state.a;
    double keL = 0.5*(uL*uL + vL*vL + wL*wL);
    double HL = eL + pLrL + keL;
    //
    double rR = right.gas_state.rho;
    double pR = right.gas_state.p;
    double pRrR = pR / rR;
    double uR = right.velocity.x;
    double vR = right.velocity.y;
    double wR = right.velocity.z;
    double eR = right.gas_state.u;
    double aR = right.gas_state.a;
    double keR = 0.5*(uR*uR + vR*vR + wR*wR);
    double HR = eR + pRrR + keR;
    //
    // This is the main part of the flux calculator.
    // Weighting parameters (eqn 32) for velocity splitting.
    double alphaL = 2.0 * pLrL / (pLrL + pRrR);
    double alphaR = 2.0 * pRrR / (pLrL + pRrR);
    // Common sound speed (eqn 33) and Mach doubles.
    double am = fmax(aL, aR);
    double ML = uL / am;
    double MR = uR / am;
    // Left state:
    // pressure splitting (eqn 34)
    // and velocity splitting (eqn 30)
    double pLplus, uLplus;
    double duL = 0.5 * (uL + fabs(uL));
    if (fabs(ML) <= 1.0) {
        pLplus = pL * (ML + 1.0) * (ML + 1.0) * (2.0 - ML) * 0.25;
        uLplus = alphaL * ((uL + am) * (uL + am) / (4.0 * am) - duL) + duL;
    } else {
        pLplus = pL * duL / uL;
        uLplus = duL;
    }
    // Right state:
    // pressure splitting (eqn 34)
    // and velocity splitting (eqn 31)
    double pRminus, uRminus;
    double duR = 0.5 * (uR - fabs(uR));
    if (fabs(MR) <= 1.0) {
        pRminus = pR * (MR - 1.0) * (MR - 1.0) * (2.0 + MR) * 0.25;
        uRminus = alphaR * (-(uR - am) * (uR - am) / (4.0 * am) - duR) + duR;
    } else {
        pRminus = pR * duR / uR;
        uRminus = duR;
    }
    // Mass Flux (eqn 29)
    // The mass flux is relative to the moving interface.
    double ru_half = uLplus * rL + uRminus * rR;
    // Pressure flux (eqn 34)
    double p_half = pLplus + pRminus;
    // Momentum flux: normal direction
    // Compute blending parameter s (eqn 37),
    // the momentum flux for AUSMV (eqn 21) and AUSMD (eqn 21)
    // and blend (eqn 36).
    double dp = pL - pR;
    const double K_SWITCH = 10.0;
    dp = K_SWITCH * fabs(dp) / fmin(pL, pR);
    double s = 0.5 * fmin(1.0, dp);
    double ru2_AUSMV = uLplus * rL * uL + uRminus * rR * uR;
    double ru2_AUSMD = 0.5 * (ru_half * (uL + uR) - fabs(ru_half) * (uR - uL));
    double ru2_half = (0.5 + s) * ru2_AUSMV + (0.5 - s) * ru2_AUSMD;
    //
    // Assemble components of the flux vector.
    flux[flux.rho()] += ru_half;
    if (ru_half >= 0.0) {
        // Wind is blowing from the left.
        flux[flux.momentum()] += (ru2_half+p_half);
        flux[flux.momentum()+1] += (ru_half*vL);
        if (flux.dimensions() == 3) { flux[flux.momentum()+2] += (ru_half*wL); }
        flux[flux.energy()] += ru_half*HL;
    } else {
        // Wind is blowing from the right.
        flux[flux.momentum()] += (ru2_half+p_half);
        flux[flux.momentum()+1] += (ru_half*vR);
        if (flux.dimensions() == 3) { flux[flux.momentum()+2] += (ru_half*wR); }
        flux[flux.energy()] += ru_half*HR;
    }

    // Apply entropy fix (section 3.5 in Wada and Liou's paper)
    const double C_EFIX = 0.125;
    bool caseA = ((uL - aL) < 0.0) && ((uR - aR) > 0.0);
    bool caseB = ((uL + aL) < 0.0) && ((uR + aR) > 0.0);
    //
    double d_ua = 0.0;
    if (caseA && !caseB) { d_ua = C_EFIX * ((uR - aR) - (uL - aL)); }
    if (caseB && !caseA) { d_ua = C_EFIX * ((uR + aR) - (uL + aL)); }
    if (d_ua != 0.0) {
        flux[flux.rho()] -= d_ua*(rR - rL);
        flux[flux.momentum()] -= d_ua*(rR*uR - rL*uL);
        flux[flux.momentum()+1] -= d_ua*(rR*vR - rL*vL);
        if (flux.dimensions() > 2) { 
            flux[flux.momentum()+2] -= d_ua*(rR*wR - rL*wL); }
        flux[flux.energy()] -= d_ua*(rR*HR - rL*HL);
    }
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
    double pRrR = pR / rR;
    double uR = right.velocity.x;
    double vR = right.velocity.y;
    double wR = right.velocity.z;
    double eR = right.gas_state.u;
    double aR = right.gas_state.a;
    double keR = 0.5 * (uR*uR + vR*vR + wR*wR);
    double HR = eR + pRrR + keR;

    // left state pressure and velocity splitting
    double pLplus, uLplus;
    if (fabs(uL) <= aL){
        uLplus = 1.0/(4.0*aL) * (uL+aL)*(uL+aL);
        pLplus = pL*uLplus * (1.0/aL * (2.0-uL/aL));
    }
    else{
        uLplus = 0.5*(uL+fabs(uL));
        pLplus = pL*uLplus * (1.0/uL);
    }

    // right state pressure and velocity splitting
    double pRminus, uRminus;
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
    flux[momentum]   = uLplus * rL * uL + uRminus * rR * uR + p_half;
    flux[momentum+1] = uLplus * rL * vL + uRminus * rR * vR;
    if (flux.dimensions() > 2){
        flux[momentum+2] = uLplus * rL * wL + uRminus * rR * wR;
    }
    flux[flux.energy()] = uLplus * rL * HL + uRminus * rR * HR; 
}
#ifdef GPU
#pragma omp end declare target
#endif
}
