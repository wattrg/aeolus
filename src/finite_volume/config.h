#ifndef __CONFIG_H
#define __CONFIG_H

#include "boundary_conditions/boundary_condition.h"
#include "../io/io.h"
#include "flux_calc.h"
#include "../solvers/solvers.h"
#include "../grid/grid.h"
#include <sys/stat.h>
#include <map>
#include <functional>


class FluidBlock;
class FluidBlockIO;

class Solver;

class Simulation{

public:
    Simulation();
    ~Simulation();
    void add_fluid_block(
        Grid::Grid & grid, 
        FlowState &,
        std::map<std::string, BoundaryCondition> &
    );
    void add_fluid_block(
        Grid::Grid & grid,
        std::function<FlowState(double, double, double)> &,
        std::map<std::string, BoundaryCondition> &
    );
    unsigned short dimensions() const;
    void set_dimensions(unsigned short number_dim);
    std::vector<FluidBlock *> & fluid_blocks();
    FluxCalculators flux_calculator() {return _flux_calculator;}
    void set_flux_calculator(FluxCalculators flux_calc) {_flux_calculator = flux_calc;}
    void write_fluid_blocks();
    void run();
    const Solver & solver () const;
    void add_solver(Solver & solver);
    GasModel & g_model() {return _g_model;}
    void set_gas_model(GasModel gas_model) {_g_model = gas_model;}
    void add_time_increment(double dt) {_time += dt;}
    double time() const {return _time;}
    Logger log;

private:
    int _time_index = 0;
    double _time = 0.0;
    unsigned short _dimensions = 2;
    std::vector<FluidBlock *> _fluid_blocks;
    FluidBlockIO * _fluid_block_io = nullptr;
    FluxCalculators _flux_calculator;
    GasModel _g_model;
    std::vector<Solver *> _solvers;
    unsigned int _solver_idx=0;
};

#endif
