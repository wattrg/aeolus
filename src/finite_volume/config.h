#ifndef __CONFIG_H
#define __CONFIG_H

#include "io/fluid_block_io.h"
#include "fluid_block.h"
#include "boundary_conditions/boundary_condition.h"
#include <sys/stat.h>
#include <map>


class FluidBlock;
class FluidBlockIO;

class Simulation{

public:
    Simulation();
    ~Simulation();
    void add_fluid_block(const char *, std::map<std::string, BoundaryCondition> &);
    const unsigned short dimensions() const;
    void set_dimensions(unsigned short number_dim);
    std::vector<FluidBlock *> & fluid_blocks();
    FluxCalculators::FluxCalculators flux_calculator() {return _flux_calculator;}
    void set_flux_calculator(FluxCalculators::FluxCalculators flux_calc) {_flux_calculator = flux_calc;}
    void write_fluid_blocks();
    void solve();
    GasModel & g_model() {return _g_model;}
    void set_gas_model(GasModel gas_model) {_g_model = gas_model;}

private:
    int _time_index = 0;
    unsigned short _dimensions = 2;
    std::vector<FluidBlock *> _fluid_blocks;
    FluidBlockIO * _fluid_block_io = nullptr;
    FluxCalculators::FluxCalculators _flux_calculator;
    GasModel _g_model;
    //std::vector<Solver> _solver;
};

#endif
