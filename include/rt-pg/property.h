#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace rtpg {

enum class PropertyType { Bool, Int, Double, String };
struct Property {
  std::string name;
  PropertyType type;
  std::variant<bool, int64_t, double, std::string> data;
};

}  // namespace rtpg
