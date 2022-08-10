from aeolus import FluidBlock, GasState, GasModel, FlowState, Vector3, FluidBlockIO, GlobalConfig
import aeolus.bc

gm = GasModel(287)

def fill_func(x, y, _):
    """ compute the flow state as a function of position """
    gas_state = GasState()
    if x < 1.5:
        gas_state.T = 500
    else:
        gas_state.T = 300
    gas_state.p = 101325
    gm.update_from_pT(gas_state)
    vel = Vector3(1000+200*x, 500*(y-1.5))
    return FlowState(gas_state, vel)

config = GlobalConfig()
config.dimensions = 2

config.add_fluid_block("test_grid.su2")

fb_io = FluidBlockIO()
config.fluid_blocks[0].fill_function(fill_func)
fb_io.write_fluid_block("test.vtu", config.fluid_blocks[0])
