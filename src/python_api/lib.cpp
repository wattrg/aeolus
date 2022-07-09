#include "../gas_state.h"
#include "../gas_model.h"
#include <pybind11/pybind11.h>


PYBIND11_MODULE(aeolus, m) {
    m.doc() = "Aeolus gas module";

    pybind11::class_<GasState>(m, "GasState")
        .def(pybind11::init())
        .def_readwrite("p", &GasState::p)
        .def_readwrite("T", &GasState::T)
        .def_readwrite("rho", &GasState::rho);

    pybind11::class_<GasModel>(m, "GasModel")
        .def(pybind11::init<double>())
        .def("update_from_pT", &GasModel::update_from_pT)
        .def("update_from_rhoT", &GasModel::update_from_rhoT)
        .def("update_from_prho", &GasModel::update_from_prho);
        //.def("internal_energy", &GasModel::internal_energy);

}
