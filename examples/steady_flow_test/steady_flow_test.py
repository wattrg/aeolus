import faulthandler
faulthandler.enable()
from aeolus import GasState, GasModel, FlowState, Vector3, Simulation, ExplicitSolver, FluxCalculators, Grid, FluidBlock
from aeolus.bc import SlipWall, SupersonicInflow, SupersonicOutflow

gm = GasModel(287)

def fill_func(x, y, _):
    """ compute the flow state as a function of position """
    gas_state = GasState()
    gas_state.T = 300
    gas_state.p = 101325/2
    gm.update_from_pT(gas_state)
    vel = Vector3(1000, 100)
    return FlowState(gas_state, vel)

sim = Simulation()
sim.dimensions = 2
sim.flux_calculator = FluxCalculators.hanel

inflow_gs = GasState()
inflow_gs.T = 2*300
inflow_gs.p = 101325
gm.update_from_pT(inflow_gs)
inflow = FlowState(inflow_gs, Vector3(1000))

grid = Grid("test_grid.su2")
bcs = {
    "slip_wall": SlipWall(),
    "outflow": SupersonicOutflow(),
    "inflow": SupersonicInflow(inflow)
}
fb = FluidBlock(grid, 0, bcs)
print(fb)
fluid_block = FluidBlock(grid, 0, bcs)

sim.gas_model = gm
sim.add_fluid_block(grid, fill_func, bcs)
sim.write_fluid_blocks()

solver = ExplicitSolver(sim)
solver.cfl = 0.5
solver.max_step = 1000
solver.print_step = 50
solver.plot_step = 100

sim.add_solver(solver)
sim.run()
