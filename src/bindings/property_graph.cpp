#include "rt-pg/property_graph.h"

// Include opaque_types.h as we use stl.h and need to avoid violating the one
// definition rule
#include "opaque_types.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;

namespace {
using UUID = uuids::uuid;
using PropertyGraph = rtpg::PropertyGraph;
using Properties = rtpg::Properties;
using Handle = handle_allocator::Handle;
}  // namespace

void initPropertyGraph(py::module_ &m) {
  py::class_<rtpg::PropertyGraph>(m, "PropertyGraph")
      .def(py::init<>())
      .def("AddNode",
           py::overload_cast<std::vector<UUID>>(&PropertyGraph::AddNode))
      .def("AddNode",
           py::overload_cast<UUID, std::vector<UUID>>(&PropertyGraph::AddNode))
      .def("AddNode", py::overload_cast<std::vector<UUID>, const Properties &>(
                          &PropertyGraph::AddNode))
      .def("AddNode",
           py::overload_cast<UUID, std::vector<UUID>, const Properties &>(
               &PropertyGraph::AddNode))
      .def("AddEdge",
           py::overload_cast<UUID, UUID, UUID>(&PropertyGraph::AddEdge))
      .def("AddEdge", py::overload_cast<const Handle &, const Handle &, UUID>(
                          &PropertyGraph::AddEdge))
      .def("AddEdge", py::overload_cast<UUID, UUID, UUID, const Properties &>(
                          &PropertyGraph::AddEdge))
      .def("AddEdge",
           py::overload_cast<const Handle &, const Handle &, UUID,
                             const Properties &>(&PropertyGraph::AddEdge))
      .def("RemoveNode", py::overload_cast<UUID>(&PropertyGraph::RemoveNode))
      .def("RemoveNode",
           py::overload_cast<const Handle &>(&PropertyGraph::RemoveNode))
      .def("RemoveEdge", py::overload_cast<UUID>(&PropertyGraph::RemoveEdge))
      .def("RemoveEdge",
           py::overload_cast<const Handle &>(&PropertyGraph::RemoveEdge))
      .def("GetConstNodePtr",
           py::overload_cast<UUID>(&PropertyGraph::GetConstNodePtr))
      .def("GetNodePtr", py::overload_cast<UUID>(&PropertyGraph::GetNodePtr))
      .def("GetConstNodePtr",
           py::overload_cast<const Handle &>(&PropertyGraph::GetConstNodePtr))
      .def("GetNodePtr",
           py::overload_cast<const Handle &>(&PropertyGraph::GetNodePtr))
      .def("GetConstEdgePtr",
           py::overload_cast<UUID>(&PropertyGraph::GetConstEdgePtr))
      .def("GetEdgePtr", py::overload_cast<UUID>(&PropertyGraph::GetEdgePtr))
      .def("GetConstEdgePtr",
           py::overload_cast<const Handle &>(&PropertyGraph::GetConstEdgePtr))
      .def("GetEdgePtr",
           py::overload_cast<const Handle &>(&PropertyGraph::GetEdgePtr));
}