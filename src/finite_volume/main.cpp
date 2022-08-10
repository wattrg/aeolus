#include <iostream>
#include <fstream>
#include "../gas/gas_state.h"
#include "../gas/gas_model.h"
#include "fluid_block.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello world!\n";

    GasState gas_state = GasState();
    GasModel g_model = GasModel(287.0);

    gas_state.p = 1e5;
    gas_state.T = 300;
    g_model.update_from_pT(gas_state);

    Simulation config = Simulation();

    config.add_fluid_block("test_grid.su2");
    //FluidBlock fluid_block = FluidBlock("test_grid.su2", config, 0);
    FluidBlockIO fb_io = FluidBlockIO();
    fb_io.write_fluid_block("test.vtu", *config.fluid_blocks()[0]);
    std::cout << "Finished :)\n";

    return 0;
}
