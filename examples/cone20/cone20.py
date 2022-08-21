import faulthandler
faulthandler.enable()
from aeolus import GasState, GasModel, FlowState, Vector3, Simulation, ExplicitSolver, FluxCalculators
from aeolus.bc import SlipWall, SupersonicInflow, SupersonicOutflow

gm = GasModel(287)

sim = Simulation()
sim.dimensions = 2
sim.flux_calculator = FluxCalculators.hanel

inflow_gs = GasState()
inflow_gs.p = 95.84e3
inflow_gs.T = 1103.0

gm.update_from_pT(inflow_gs)
inflow = FlowState(inflow_gs, Vector3(500.0))

bcs = {
    "SLIP_WALL": SlipWall(),
    "OUTFLOW": SupersonicOutflow(),
    "INFLOW": SupersonicInflow(inflow)
}

sim.gas_model = GasModel(287)
sim.add_fluid_block("cone20.su2", inflow, bcs)

solver = ExplicitSolver(sim)
solver.max_step = 10000
solver.cfl = 0.1
solver.print_step = 100
solver.plot_step = 1000

sim.write_fluid_blocks()

sim.add_solver(solver)
sim.run()
