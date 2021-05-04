#pragma once
// Documentation for file.

#include <memory>
#include <vector>

#include "rt-pg/graph_element.h"
#include "rt-pg/types.h"

namespace rtpg {

// declarations
class Node : public GraphElement {
 public:
  Node(UUID id, const std::vector<UUID>& mids, const Properties& properties);
  ~Node();
  const std::vector<UUID>& mids() const;
  std::vector<UUID>& mids();
  const std::vector<Handle>& out_edges() const;
  // FIXME: Do we need to give modifiable edge lists?
  std::vector<Handle>& out_edges();
  const std::vector<Handle>& in_edges() const;
  // FIXME: Do we need to give modifiable edge lists?
  std::vector<Handle>& in_edges();

 protected:
  std::vector<UUID> mids_;
  std::vector<Handle> out_edges_;
  std::vector<Handle> in_edges_;
};

using ConstNodePtr = std::shared_ptr<const Node>;
using NodePtr = std::shared_ptr<Node>;

}  // namespace rtpg
