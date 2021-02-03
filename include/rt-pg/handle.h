#pragma once

#include <cstddef>

namespace rtpg {
struct Handle {
  std::size_t index;
  std::size_t unique_pattern;
};

}  // namespace rtpg
