#ifndef __SOLVERS_H_
#define __SOLVERS_H_

#include "../finite_volume/config.h"

class Solver {
public:
    Solver(Simulation & config);
    void solve();

protected:
    virtual void _step()=0;
    Simulation _config;
};


#endif
