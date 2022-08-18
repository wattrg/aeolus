import faulthandler
faulthandler.enable()
from aeolus import GasState, GasModel, FlowState, Vector3, Simulation, ExplicitSolver, FluxCalculators
from aeolus.bc import SlipWall, SupersonicInflow, SupersonicOutflow

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

sim = Simulation()
sim.dimensions = 2
sim.flux_calculator = FluxCalculators.hanel

bcs = {
    "slipwall": SlipWall(),
    "outflow": SupersonicOutflow(),
    "inflow": SupersonicInflow(fill_func(0,0,0))
}

sim.gas_model = GasModel(287)
sim.add_fluid_block("test_grid.su2", bcs)

sim.fluid_blocks[0].fill_function(fill_func)
sim.write_fluid_blocks()

solver = ExplicitSolver(sim)
solver.max_step = 1

solver.solve()
sim.write_fluid_blocks()
