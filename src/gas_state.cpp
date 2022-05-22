#include "gas_state.h"

std::ostream& operator << (std::ostream& os, const GasState gs){
    os << "GasState(";
    os << "p = " << gs.p << "(Pa), ";
    os << "rho = " << gs.rho << "(kg/m^3), ";
    os << "T = " << gs.T << "(K), ";
    os << "u = " << gs.u << "(J))";
    return os;
}
