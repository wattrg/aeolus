#include <iostream>
#include <fstream>
#include "../gas/gas_state.h"
#include "../gas/gas_model.h"
#include "fluid_block.h"
#include "boundary_conditions/boundary_condition.h"

FlowState initial_conditions(double x, double y, double z){
    GasModel gm = GasModel(287.0);
    GasState gs = GasState(gm);
    gs.p = 101325.0;
    gs.T = 300.0;
    gm.update_from_pT(gs);
    FlowState fs = FlowState(gs, Vector3(1000, 100));
    return fs;
}

int main(int argc, char *argv[]) {
    std::cout << "Hello world!\n";

    GasState gas_state = GasState();
    GasModel g_model = GasModel(287.0);

    gas_state.p = 1e5;
    gas_state.T = 300;
    g_model.update_from_pT(gas_state);
    FlowState flow_state = FlowState(gas_state, Vector3(1000.0));

    Simulation config = Simulation();

    std::map<std::string, BoundaryCondition> bc_map;
    bc_map.insert(std::pair<std::string, BoundaryCondition>("slip_wall", SlipWall("slip_wall")));
    bc_map.insert(std::pair<std::string, BoundaryCondition>("outflow", SupersonicOutflow("outflow")));
    bc_map.insert(std::pair<std::string, BoundaryCondition>("inflow", SupersonicInflow(flow_state, "inflow")));

    config.add_fluid_block("test_grid.su2", bc_map);
    std::function<FlowState(double, double, double)> ic = initial_conditions;
    config.fluid_blocks()[0]->fill_function(ic);
    config.write_fluid_blocks();//("test.vtu", *config.fluid_blocks()[0]);
    std::cout << "Finished :)\n";

    return 0;
}
