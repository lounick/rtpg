#include "bindings/common.h"
#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
// For handling opaque types
#include "opaque_types.h"

namespace py = pybind11;

void initGraphElement(py::module_ &);
void initUUID(py::module_ &);
void initHandle(py::module_ &);
void initNode(py::module_ &);
void initEdge(py::module_ &);
void initPropertyGraph(py::module_ &);

PYBIND11_MODULE(MODULE_NAME, m) {
  initGraphElement(m);
  initUUID(m);
  initHandle(m);
  initNode(m);
  initEdge(m);
  initPropertyGraph(m);

  py::bind_vector<std::vector<rtpg::UUID>>(m, "VectorUUID");
  py::bind_vector<std::vector<rtpg::Handle>>(m, "VectorHandle");
  auto map = py::bind_map<rtpg::Properties>(m, "MapProperties");
  map.def(
      "__eq__",
      [](const rtpg::Properties &a, const rtpg::Properties &b) {
        return a == b;
      },
      py::is_operator());
#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}