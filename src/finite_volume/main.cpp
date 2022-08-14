#include <iostream>
#include <fstream>
#include "../gas/gas_state.h"
#include "../gas/gas_model.h"
#include "../solvers/explicit.h"
#include "fluid_block.h"
#include "boundary_conditions/boundary_condition.h"

FlowState initial_conditions(double x, double y, double z){
    GasModel gm = GasModel(287.0);
    GasState gs = GasState(gm);
    if (x < 1.5){
        gs.p = 101325.0;
        gs.T = 300.0;
    }
    else{
        gs.p = 1e6;
        gs.T = 1000.0;
    }
    gm.update_from_pT(gs);
    FlowState fs = FlowState(gs, Vector3(1000));
    return fs;
}

int main(int argc, char *argv[]) {
    std::cout << "Hello world!\n";

    GasModel g_model = GasModel(287.0);

    Simulation config = Simulation();
    config.set_flux_calculator(FluxCalculators::hanel);

    GasState inflow_gs = GasState();
    inflow_gs.p = 1e6;
    inflow_gs.T = 1000;
    g_model.update_from_pT(inflow_gs);
    FlowState inflow = FlowState(inflow_gs, Vector3(2000.0));

    std::map<std::string, BoundaryCondition> bc_map;
    bc_map.insert(std::pair<std::string, BoundaryCondition>("slip_wall", SlipWall("slip_wall")));
    bc_map.insert(std::pair<std::string, BoundaryCondition>("outflow", SupersonicOutflow("outflow")));
    bc_map.insert(std::pair<std::string, BoundaryCondition>("inflow", SupersonicInflow(inflow, "inflow")));

    config.add_fluid_block("test_grid.su2", bc_map);
    std::function<FlowState(double, double, double)> ic = initial_conditions;
    config.fluid_blocks()[0]->fill_function(ic);
    config.write_fluid_blocks();

    ExplicitSolver solver = ExplicitSolver(config);
    solver.set_max_step(1);
    solver.solve();

    config.write_fluid_blocks();

    std::cout << "Finished :)\n";

    return 0;
}
