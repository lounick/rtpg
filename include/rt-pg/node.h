#pragma once
// Documentation for file.

#include <vector>

#include "rt-pg/graph_element.h"
#include "rt-pg/property.h"
#include "rt-pg/types.h"

namespace rtpg {

// declarations
class Node : public GraphElement {
 public:
  Node();
  Node(UUID id, std::vector<UUID> mids, Properties properties);
  ~Node();
  const std::vector<UUID>& mids() const;
  const std::vector<Handle>& out_edges() const;
  std::vector<Handle>& out_edges();
  const std::vector<Handle>& in_edges() const;
  std::vector<Handle>& in_edges();
  const Properties& properties() const;
  Properties& properties();

 protected:
  std::vector<UUID> mids_;
  std::vector<Handle> out_edges_;
  std::vector<Handle> in_edges_;
};

}  // namespace rtpg
