import faulthandler
faulthandler.enable()
from aeolus import GasState, GasModel, FlowState, Vector3, Simulation, ExplicitSolver, FluxCalculators
from aeolus.bc import SlipWall, SupersonicInflow, SupersonicOutflow

gm = GasModel(287)

sim = Simulation()
sim.dimensions = 2
sim.flux_calculator = FluxCalculators.hanel

inflow_gs = GasState()
inflow_gs.p = 1e5
inflow_gs.T = 300.0
gm.update_from_pT(inflow_gs)
inflow = FlowState(inflow_gs, Vector3(inflow_gs.a*10))

bcs = {
    "south": SlipWall(),
    "north": SupersonicOutflow(),
    "west": SupersonicOutflow(),
    "east": SupersonicInflow(inflow)
}

sim.gas_model = GasModel(287)
sim.add_fluid_block("wedge6_coarse.su2", FlowState(inflow_gs, Vector3(1000)), bcs)

solver = ExplicitSolver(sim)
solver.max_step = 1000
solver.cfl = 0.5
solver.print_step = 100
solver.plot_step = 10

sim.write_fluid_blocks()

sim.add_solver(solver)
sim.run()
