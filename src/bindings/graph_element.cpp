#include "rt-pg/graph_element.h"

#include "opaque_types.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;

void initGraphElement(py::module_ &m) {
  py::class_<rtpg::GraphElement, std::shared_ptr<rtpg::GraphElement>>(
      m, "GraphElement")
      .def(py::init<uuids::uuid>())
      .def(py::init<uuids::uuid, const rtpg::Properties &>())
      .def("id", &rtpg::GraphElement::id, py::return_value_policy::reference)
      .def("properties", py::overload_cast<>(&rtpg::GraphElement::properties),
           py::return_value_policy::reference)
      .def("const_properties",
           py::overload_cast<>(&rtpg::GraphElement::properties, py::const_),
           py::return_value_policy::reference);
}