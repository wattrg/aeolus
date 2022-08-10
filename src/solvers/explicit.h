#ifndef __EULER_SOLVER_H_
#define __EULER_SOLVER_H_

#include "solvers.h"

class ExplicitSolver : Solver {
public:
    void solve();

protected:
    void step();
};

#endif
