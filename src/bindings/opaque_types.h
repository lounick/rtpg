#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"
#include "rt-pg/types.h"

PYBIND11_MAKE_OPAQUE(std::vector<rtpg::UUID>);
PYBIND11_MAKE_OPAQUE(std::vector<rtpg::Handle>);
PYBIND11_MAKE_OPAQUE(rtpg::Properties);