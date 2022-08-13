#ifndef __EULER_SOLVER_H_
#define __EULER_SOLVER_H_

#include "solvers.h"

class ExplicitSolver : public Solver {
public:
    ExplicitSolver(Simulation & config) : Solver(config) {};

protected:
    void _step();
};

#endif
