#include "uuid.h"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;

void initUUID(py::module_ &m) {
  py::enum_<uuids::uuid_variant>(m, "UUIDVariant")
      .value("NCS", uuids::uuid_variant::ncs)
      .value("RFC", uuids::uuid_variant::rfc)
      .value("Microsoft", uuids::uuid_variant::microsoft)
      .value("Reserved", uuids::uuid_variant::reserved);

  py::enum_<uuids::uuid_version>(m, "UUIDVersion")
      .value("None", uuids::uuid_version::none)
      .value("TimeBased", uuids::uuid_version::time_based)
      .value("DCESecurity", uuids::uuid_version::dce_security)
      .value("NameBasedMD5", uuids::uuid_version::name_based_md5)
      .value("RandomNumberBased", uuids::uuid_version::random_number_based)
      .value("NameBasedSHA1", uuids::uuid_version::name_based_sha1);

  py::class_<uuids::uuid>(m, "UUID")
      .def(py::init<>())
      .def(py::init<std::array<uuids::uuid::value_type, 16>>())
      .def("variant", &uuids::uuid::variant)
      .def("version", &uuids::uuid::version)
      .def("is_nil", &uuids::uuid::is_nil)
      .def("swap", &uuids::uuid::swap)
      .def_static("is_valid_uuid",
                  static_cast<bool (*)(
                      std::basic_string<char, std::char_traits<char>,
                                        std::allocator<char>> const &)>(
                      &uuids::uuid::is_valid_uuid))
      .def_static("from_string",
                  static_cast<std::optional<uuids::uuid> (*)(
                      std::basic_string<char, std::char_traits<char>,
                                        std::allocator<char>> const &)>(
                      &uuids::uuid::from_string))
      .def("to_string",
           static_cast<std::basic_string<char, std::char_traits<char>,
                                         std::allocator<char>> (*)(
               uuids::uuid const &)>(&uuids::to_string));
}