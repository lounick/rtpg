#include "rt-pg/edge.h"

namespace rtpg {
Edge::Edge(UUID id, UUID mid, Handle head, Handle tail,
           const Properties& properties)
    : GraphElement(id, properties), mid_{mid}, head_{head}, tail_{tail} {}
Edge::~Edge() {}
const UUID& Edge::mid() const { return mid_; }
const Handle& Edge::head() const { return head_; }
const Handle& Edge::tail() const { return tail_; }
}  // namespace rtpg
