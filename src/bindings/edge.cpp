#include "rt-pg/edge.h"

#include "opaque_types.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;

void initEdge(py::module_ &m) {
  py::class_<rtpg::Edge, std::shared_ptr<rtpg::Edge>, rtpg::GraphElement>(
      m, "Edge")
      .def(py::init<uuids::uuid, uuids::uuid, handle_allocator::Handle,
                    handle_allocator::Handle, const rtpg::Properties &>())
      .def("mid", &rtpg::Edge::mid, py::return_value_policy::reference)
      .def("head", &rtpg::Edge::head, py::return_value_policy::reference)
      .def("tail", &rtpg::Edge::tail, py::return_value_policy::reference);
}