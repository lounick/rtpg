// Project Name - Project brief description
// Copyright (c) 2015 Primary Author and contributors
//
// Licensed under the Whatever License blah blah no warranties.

#pragma once
// Documentation for file.

#include <vector>

#include "rt-pg/graph_element.h"
#include "rt-pg/handle.h"
#include "rt-pg/property.h"
#include "rt-pg/types.h"

namespace rtpg {

// declarations
class Node : public GraphElement {
 public:
  Node(UUID id, std::vector<UUID> mids);
  ~Node();
  const std::vector<UUID>& mids() const;
  const std::vector<Handle>& out_edges() const;
  const std::vector<Handle>& in_edges() const;

 protected:
  std::vector<UUID> mids_;
  std::vector<Handle> out_edges_;
  std::vector<Handle> in_edges_;
};

struct NodeItem {
  Handle handle;
  Node node;
};

namespace _ {  // private

// private declarations

}  // namespace _

}  // namespace rtpg