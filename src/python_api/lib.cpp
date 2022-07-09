#include "../gas_state.h"
#include "../gas_model.h"
#include "../flow_state.h"
#include "../vector.h"
#include "../fluid_block.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>


PYBIND11_MODULE(aeolus, m) {
    m.doc() = "Aeolus gas module";

    pybind11::class_<GasState>(m, "GasState")
        .def(pybind11::init())
        .def_readwrite("p", &GasState::p)
        .def_readwrite("T", &GasState::T)
        .def_readwrite("rho", &GasState::rho)
        .def("__repr__", &GasState::to_string);

    pybind11::class_<GasModel>(m, "GasModel")
        .def(pybind11::init<double>())
        .def("update_from_pT", &GasModel::update_from_pT)
        .def("update_from_rhoT", &GasModel::update_from_rhoT)
        .def("update_from_prho", &GasModel::update_from_prho);

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
        .def_readwrite("gas_state", &FlowState::gas_state)
        .def_readwrite("velocity", &FlowState::velocity)
        .def("__repr__", &FlowState::to_string);

    pybind11::class_<FluidBlock>(m, "FluidBlock")
        .def(pybind11::init<const char *>())
        .def("__repr__", &FluidBlock::to_string)
        .def("fill_function", &FluidBlock::fill_function);
}
