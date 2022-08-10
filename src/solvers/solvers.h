#ifndef __SOLVERS_H_
#define __SOLVERS_H_

#include "../finite_volume/config.h"

class Solver {
public:
    Solver(GlobalConfig & config);
    void solve();

protected:
    virtual void _step()=0;
    GlobalConfig _config;
};


#endif
