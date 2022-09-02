#include "../gas/gas_state.h"
#include "../gas/gas_model.h"
#include "../gas/flow_state.h"
#include "../util/vector3.h"
#include "../finite_volume/fluid_block.h"
#include "../finite_volume/io/fluid_block_io.h"
#include "../solvers/explicit.h"
#include "../util/header.h"
#include "omp.h"
//#include "../finite_volume/boundary_conditions/boundary_conditions.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>


PYBIND11_MODULE(aeolus, m) {
    m.doc() = "Aeolus python module";

    pybind11::class_<GasState>(m, "GasState")
        .def(pybind11::init())
        .def_readwrite("p", &GasState::p, "pressure (Pa)")
        .def_readwrite("T", &GasState::T, "temperature (K)")
        .def_readwrite("rho", &GasState::rho, "density (kg/m^3)")
        .def_readwrite("a", &GasState::a, "Speed of sound")
        .def("__repr__", &GasState::to_string);

    pybind11::class_<GasModel>(m, "GasModel")
        .def(pybind11::init<double>())
        .def_property("gamma", &GasModel::gamma, nullptr, "Ratio of specific heats")
        .def("update_from_pT", &GasModel::update_from_pT, "update thermodynamic state assuming pressure and temperature are correct")
        .def("update_from_rhoT", &GasModel::update_from_rhoT, "update thermodynamic state assuming density and temperature are correct")
        .def("update_from_prho", &GasModel::update_from_prho, "update thermodynamics state assuming pressue and density are correct");

    pybind11::class_<Vector3>(m, "Vector3")
        .def(pybind11::init<double>())
        .def(pybind11::init<double, double>())
        .def(pybind11::init<double, double, double>())
        .def_readwrite("x", &Vector3::x)
        .def_readwrite("y", &Vector3::y)
        .def_readwrite("z", &Vector3::z)
        .def("__repr__", &Vector3::to_string);

    pybind11::class_<FlowState>(m, "FlowState")
        .def(pybind11::init())
        .def(pybind11::init<GasState, Vector3>())
        .def_readwrite("gas_state", &FlowState::gas_state, "the thermodynamic state of the flow")
        .def_readwrite("velocity", &FlowState::velocity, "the velocity of the flow")
        .def("__repr__", &FlowState::to_string);

    pybind11::class_<Simulation>(m, "Simulation")
        .def(pybind11::init())
        .def_property("dimensions", &Simulation::dimensions, &Simulation::set_dimensions, "The number of spatial dimensions")
        .def_property("fluid_blocks", &Simulation::fluid_blocks, nullptr, "The fluid blocks")
        .def_property("flux_calculator", &Simulation::flux_calculator, &Simulation::set_flux_calculator, "The flux calculator")
        .def_property("gas_model", &Simulation::g_model, &Simulation::set_gas_model, "The gas model")
        .def("add_fluid_block", 
             static_cast<void (Simulation::*)(const char *, FlowState &, std::map<std::string, BoundaryCondition>&)>(&Simulation::add_fluid_block), 
             "Add a fluid block")
        .def("add_fluid_block", 
             static_cast<void (Simulation::*)(const char *, std::function<FlowState(double, double, double)>&, std::map<std::string, BoundaryCondition>&)>(&Simulation::add_fluid_block), 
             "Add a fluid block")
        .def("add_solver", &Simulation::add_solver, "Add a solver")
        .def("run", &Simulation::run, "Run the simulation")
        .def("write_fluid_blocks", &Simulation::write_fluid_blocks, "Write the fluid blocks to file");

    pybind11::class_<FluidBlock>(m, "FluidBlock")
        .def(pybind11::init<const char *, Simulation &, unsigned int, std::map<std::string, BoundaryCondition> &>())
        .def("__repr__", &FluidBlock::to_string);

    pybind11::class_<FluidBlockIO>(m, "FluidBlockIO")
        .def(pybind11::init<>())
        .def("write_fluid_block", &FluidBlockIO::write_fluid_block, "Write the fluid block to file");

    pybind11::class_<Solver>(m, "Solver")
        .def_property("cfl", &Solver::cfl, &Solver::set_cfl, "CFL number")
        .def_property("max_step", &Solver::max_step, &Solver::set_max_step, "maximum number of steps")
        .def_property("print_step", &Solver::print_step, &Solver::set_print_step, "How often to print the progress")
        .def_property("plot_step", &Solver::plot_step, &Solver::set_plot_step, "How many steps to write the solution");

    pybind11::class_<ExplicitSolver, Solver>(m, "ExplicitSolver")
        .def(pybind11::init<Simulation&>());

    pybind11::enum_<FluxCalculators::FluxCalculators>(m, "FluxCalculators")
        .value("hanel", FluxCalculators::FluxCalculators::hanel)
        .value("ausmdv", FluxCalculators::FluxCalculators::ausmdv)
        .export_values();


    pybind11::module_ bc = m.def_submodule("bc", "Boundary conditions");
    pybind11::class_<BoundaryCondition>(bc, "BoundaryCondition");
    pybind11::class_<SlipWall, BoundaryCondition>(bc, "SlipWall")
        .def(pybind11::init<>());

    pybind11::class_<SupersonicInflow, BoundaryCondition>(bc, "SupersonicInflow")
        .def(pybind11::init<FlowState>());

    pybind11::class_<SupersonicOutflow, BoundaryCondition>(bc, "SupersonicOutflow")
        .def(pybind11::init<>());
    
    std::cout << generate_header() << std::endl;
}
