#pragma once

#include <string>
#include <vector>

#include "rt-pg/edge.h"
#include "rt-pg/handle.h"
#include "rt-pg/node.h"
#include "rt-pg/property.h"

namespace rtpg {
class PropertyGraph;  // Forward declaration as it is required for operations.
class Traversal {
 public:
  Traversal(const PropertyGraph& g);
  Traversal& addV(std::vector<UUID> mids);
  Traversal& addE();
  std::vector<Handle> next();
  Traversal& V();
  Traversal& property(std::string key, bool value);
  Traversal& property(std::string key, int value);
  Traversal& property(std::string key, double value);
  Traversal& property(std::string key, std::string value);

 protected:
  const PropertyGraph& g_;
};
}  // namespace rtpg
