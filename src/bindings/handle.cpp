#include "handle_allocator/handle.h"

#include "pybind11/operators.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;

void initHandle(py::module_ &m) {
  py::class_<handle_allocator::Handle>(m, "Handle")
      .def(py::init<>())
      .def_readwrite("index", &handle_allocator::Handle::index)
      .def_readwrite("pattern", &handle_allocator::Handle::pattern)
      .def(py::self == py::self);
}