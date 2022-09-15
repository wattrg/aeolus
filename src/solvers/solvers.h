#ifndef __SOLVERS_H_
#define __SOLVERS_H_

#include "../finite_volume/config.h"
#include "../finite_volume/fluid_block.h"

class Simulation;

class Solver {
public:
    Solver(Simulation & config);
    virtual ~Solver() {};
    double cfl() const {return _cfl;}
    void set_cfl(double cfl) {_cfl = cfl;}
    unsigned int max_step() const {return _max_step;}
    void set_max_step(unsigned int max_step) {_max_step = max_step;}
    void set_plot_step(unsigned int plot_step) {_plot_step = plot_step;}
    unsigned int plot_step() const {return _plot_step;}
    void set_print_step(unsigned int print_step) {_print_step = print_step;}
    unsigned int print_step() const {return _print_step;}
    void solve();

protected:
    virtual double _step()=0;
    Simulation & _config;
    double _cfl = 0.5;
    unsigned int _max_step = 0;
    virtual std::string _name() = 0;
    unsigned int _plot_step = 1;
    unsigned int _print_step = 1;
};


#endif
