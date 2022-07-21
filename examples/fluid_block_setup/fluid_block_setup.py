from aeolus import FluidBlock, GasState, GasModel, FlowState, Vector3
import aeolus.bc

gm = GasModel(287)

def fill_func(x, y, z):
    gs = GasState()
    gs.T = 300
    gs.p = 101325
    gm.update_from_pT(gs)
    vel = Vector3(1000)
    return FlowState(gs, vel)


fb = FluidBlock("test_grid.su2")
fb.fill_function(fill_func)
print(fb)
