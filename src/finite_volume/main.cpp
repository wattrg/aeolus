#include <iostream>
#include <fstream>
#include "../gas/gas_state.h"
#include "../gas/gas_model.h"
#include "fluid_block.h"
#include "boundary_conditions/boundary_condition.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello world!\n";

    GasState gas_state = GasState();
    GasModel g_model = GasModel(287.0);

    gas_state.p = 1e5;
    gas_state.T = 300;
    g_model.update_from_pT(gas_state);
    FlowState flow_state = FlowState(gas_state, Vector3(1000.0));

    Simulation config = Simulation();

    std::map<std::string, BoundaryCondition *> bc_map;
    bc_map["slip_wall"] = new SlipWall("slip_wall");
    bc_map["outflow"] = new SupersonicOutflow("outflow");
    bc_map["inflow"] = new SupersonicInflow(flow_state, "inflow");

    config.add_fluid_block("test_grid.su2", bc_map);
    config.write_fluid_blocks();//("test.vtu", *config.fluid_blocks()[0]);
    std::cout << "Finished :)\n";

    return 0;
}
