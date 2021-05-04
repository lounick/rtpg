#include "rt-pg/node.h"

namespace rtpg {
Node::Node(UUID id, const std::vector<UUID>& mids, const Properties& properties)
    : GraphElement(id, properties), mids_{mids} {}
Node::~Node() {}
const std::vector<UUID>& Node::mids() const { return mids_; }
std::vector<UUID>& Node::mids() { return mids_; }
const std::vector<Handle>& Node::out_edges() const { return out_edges_; }
std::vector<Handle>& Node::out_edges() { return out_edges_; }
const std::vector<Handle>& Node::in_edges() const { return in_edges_; }
std::vector<Handle>& Node::in_edges() { return in_edges_; }
}  // namespace rtpg
