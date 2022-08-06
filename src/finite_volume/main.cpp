#include <iostream>
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


    FluidBlock fluid_block = FluidBlock("test_grid.su2");
    FluidBlockIO fb_io = FluidBlockIO();

    return 0;
}
