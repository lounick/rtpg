#include "rt-pg/node.h"

#include "opaque_types.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;

void initNode(py::module_ &m) {
  py::class_<rtpg::Node, std::shared_ptr<rtpg::Node>, rtpg::GraphElement>(
      m, "Node")
      .def(py::init<uuids::uuid, const std::vector<uuids::uuid> &,
                    const rtpg::Properties &>())
      .def("mids", py::overload_cast<>(&rtpg::Node::mids),
           py::return_value_policy::reference)
      .def("const_mids", py::overload_cast<>(&rtpg::Node::mids, py::const_),
           py::return_value_policy::reference)
      .def("out_edges", py::overload_cast<>(&rtpg::Node::out_edges),
           py::return_value_policy::reference)
      .def("const_out_edges",
           py::overload_cast<>(&rtpg::Node::out_edges, py::const_),
           py::return_value_policy::reference)
      .def("in_edges", py::overload_cast<>(&rtpg::Node::in_edges),
           py::return_value_policy::reference)
      .def("const_in_edges",
           py::overload_cast<>(&rtpg::Node::in_edges, py::const_),
           py::return_value_policy::reference);
}