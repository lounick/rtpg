#pragma once

#include <vector>

#include "rt-pg/graph_element.h"
#include "rt-pg/types.h"

namespace rtpg {
class Edge : public GraphElement {
 public:
  Edge(UUID id, UUID mid, Handle head, Handle tail,
       const Properties& properties);
  ~Edge();
  const UUID& mid() const;
  const Handle& head() const;
  const Handle& tail() const;

 protected:
  UUID mid_;
  Handle head_;
  Handle tail_;
};

}  // namespace rtpg