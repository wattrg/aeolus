#include <iostream>
#include <fstream>
#include "../gas/gas_state.h"
#include "../gas/gas_model.h"
#include "../solvers/explicit.h"
#include "fluid_block.h"
#include "boundary_conditions/boundary_condition.h"

FlowState initial_conditions(double x, double y, double z){
    (void) x; (void) y; (void) z;
    GasModel gm = GasModel(287.0);
    GasState gs = GasState(&gm);
    gs.p = 101325.0;
    gs.T = 300.0;
    gm.update_from_pT(gs);
    FlowState fs = FlowState(gs, Vector3(1000.0));
    return fs;
}

int main(int argc, char *argv[]) {
    std::cout << "Hello world!\n";
    if (argc < 2) throw std::runtime_error("You need to supply the grid file");

    GasModel g_model = GasModel(287.0);

    Simulation config = Simulation();
    config.set_flux_calculator(FluxCalculators::hanel);

    GasState inflow_gs = GasState();
    inflow_gs.p = 101325.0;
    inflow_gs.T = 300.0;
    g_model.update_from_pT(inflow_gs);
    FlowState inflow = FlowState(inflow_gs, Vector3(inflow_gs.a*10));

    std::map<std::string, BoundaryCondition> bc_map;
    bc_map.insert(std::pair<std::string, BoundaryCondition>("south", SlipWall()));
    bc_map.insert(std::pair<std::string, BoundaryCondition>("east", SupersonicOutflow()));
    bc_map.insert(std::pair<std::string, BoundaryCondition>("north", SupersonicOutflow()));
    bc_map.insert(std::pair<std::string, BoundaryCondition>("west", SupersonicInflow(inflow)));

    config.set_gas_model(g_model);
    std::function<FlowState(double, double, double)> ic = initial_conditions;
    Grid::Grid grid = Grid::Grid(argv[1]);
    config.add_fluid_block(grid, ic, bc_map);
    config.write_fluid_blocks();

    ExplicitSolver solver = ExplicitSolver(config);
    solver.set_max_step(1000);
    solver.set_print_step(100);
    solver.set_plot_step(100);
    solver.set_cfl(0.5);
    config.add_solver(solver);
    config.run();

    config.write_fluid_blocks();

    std::cout << "Finished :)\n";

    return 0;
}
