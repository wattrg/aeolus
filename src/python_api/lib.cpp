#include "../gas/gas_state.h"
#include "../gas/gas_model.h"
#include "../gas/flow_state.h"
#include "../util/vector.h"
#include "../finite_volume/fluid_block.h"
#include "../finite_volume/io/fluid_block_io.h"
//#include "../finite_volume/boundary_conditions/boundary_conditions.h"
#include "libbc.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>


PYBIND11_MODULE(aeolus, m) {
    m.doc() = "Aeolus python module";

    pybind11::class_<GasState>(m, "GasState")
        .def(pybind11::init())
        .def_readwrite("p", &GasState::p, "pressure (Pa)")
        .def_readwrite("T", &GasState::T, "temperature (K)")
        .def_readwrite("rho", &GasState::rho, "density (kg/m^3")
        .def("__repr__", &GasState::to_string);

    pybind11::class_<GasModel>(m, "GasModel")
        .def(pybind11::init<double>())
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

    pybind11::class_<FluidBlock>(m, "FluidBlock")
        .def(pybind11::init<const char *>())
        .def("__repr__", &FluidBlock::to_string)
        .def("fill_function", &FluidBlock::fill_function, "fill the fluid block with FlowState as a function of position");

    pybind11::class_<FluidBlockIO>(m, "FluidBlockIO")
        .def(pybind11::init<>())
        .def("write_fluid_block", &FluidBlockIO::write_fluid_block, "Write the fluid block to file");

    pybind11::module_ bc = m.def_submodule("bc", "Boundary conditions");
    pybind11::class_<BoundaryCondition>(bc, "BoundaryCondition");
    // m.def("slip_wall", &slip_wall, pybind11::return_value_policy::reference, "Slip wall boundary condition");
}
