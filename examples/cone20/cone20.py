import faulthandler
faulthandler.enable()
from aeolus import GasState, GasModel, FlowState, Vector3, Simulation, ExplicitSolver, FluxCalculators
from aeolus.bc import SlipWall, SupersonicInflow, SupersonicOutflow

gm = GasModel(287)

def fill_func(x, y, _):
    """ compute the flow state as a function of position """
    gas_state = GasState()
    gas_state.T = 304.0
    gas_state.p = 5955.0
    gm.update_from_pT(gas_state)
    vel = Vector3(0.0, 0.0)
    return FlowState(gas_state, vel)

sim = Simulation()
sim.dimensions = 2
sim.flux_calculator = FluxCalculators.hanel

inflow_gs = GasState()
inflow_gs.p = 95.84e3
inflow_gs.T = 1103.0

gm.update_from_pT(inflow_gs)
inflow = FlowState(inflow_gs, Vector3(1000.0))

bcs = {
    "SLIP_WALL": SlipWall(),
    "OUTFLOW": SupersonicOutflow(),
    "INFLOW": SupersonicInflow(inflow)
}

sim.gas_model = GasModel(287)
sim.add_fluid_block("cone20.su2", fill_func, bcs)

solver = ExplicitSolver(sim)
solver.max_step = 100
solver.cfl = 0.5
solver.print_step = 50
solver.plot_step = 100

sim.add_solver(solver)
sim.run()
