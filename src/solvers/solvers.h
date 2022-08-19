#ifndef __SOLVERS_H_
#define __SOLVERS_H_

#include "../finite_volume/config.h"

class Solver {
public:
    Solver(Simulation & config);
    const double cfl() const {return _cfl;}
    void set_cfl(double cfl) {_cfl = cfl;}
    const unsigned int max_step() const {return _max_step;}
    void set_max_step(unsigned int max_step) {_max_step = max_step;}
    void solve();

protected:
    virtual double _step()=0;
    Simulation & _config;
    double _cfl = 0.5;
    unsigned int _max_step = 0;
};


#endif
