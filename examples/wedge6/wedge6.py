import faulthandler
faulthandler.enable()
from aeolus import GasState, GasModel, FlowState, Vector3, Simulation, ExplicitSolver, FluxCalculators, Grid
from aeolus.bc import SlipWall, SupersonicInflow, SupersonicOutflow

gm = GasModel(287.0)

sim = Simulation()
sim.dimensions = 2
sim.flux_calculator = FluxCalculators.hanel

inflow_gs = GasState()
inflow_gs.p = 101325.0
inflow_gs.T = 300.0
gm.update_from_pT(inflow_gs)
inflow = FlowState(inflow_gs, Vector3(inflow_gs.a*10))
#inflow = FlowState(inflow_gs, Vector3(0.0))

initial_gs = GasState()
initial_gs.p = 101325.0
initial_gs.T = 300.0
gm.update_from_pT(initial_gs)
initial = FlowState(initial_gs, Vector3(1000.0))
#initial = FlowState(initial_gs, Vector3(0.0))

grid = Grid("wedge6_coarse.su2")
bcs = {
    "south": SlipWall(),
    "north": SupersonicOutflow(),
    "east": SupersonicOutflow(),
    "west": SupersonicInflow(inflow)
}

sim.gas_model = gm
sim.add_fluid_block(grid, initial, bcs)

solver = ExplicitSolver(sim)
solver.max_step = 1700
solver.cfl = 0.5
solver.print_step = 100
solver.plot_step = 100

sim.write_fluid_blocks()

sim.add_solver(solver)

sim.run()
