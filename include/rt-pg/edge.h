#pragma once

#include <vector>

#include "rt-pg/graph_element.h"
#include "rt-pg/types.h"

namespace rtpg {
class Edge : public GraphElement {
 public:
  Edge(UUID id, UUID mid);
  ~Edge();
  const UUID& mid() const;
  const Handle& head() const;
  void head(Handle head);
  const Handle& tail() const;
  void tail(Handle tail);

 protected:
  UUID mid_;
  Handle head_;
  Handle tail_;
};

}  // namespace rtpg