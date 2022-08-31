#ifndef __EULER_SOLVER_H_
#define __EULER_SOLVER_H_

#include "solvers.h"

class ExplicitSolver : public Solver {
public:
    ~ExplicitSolver(){};
    ExplicitSolver(Simulation & config) : Solver(config){};

protected:
    double _step();
    std::string _name() {return "explicit";}
};

#endif
