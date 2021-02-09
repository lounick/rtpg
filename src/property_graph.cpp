#include "rt-pg/property_graph.h"

namespace rtpg {
PropertyGraph::PropertyGraph() {
  std::random_device rd;
  gen_.reset(new std::mt19937(rd()));
  uuidv1_generator_.reset(new uuids::uuid_random_generator(gen_.get()));
}

std::optional<Handle> PropertyGraph::AddNode(std::vector<UUID> mids,
                                             Properties properites) {
  uuids::uuid id = (*uuidv1_generator_)();
  Node n(id, mids, properites);
  return nodes_.Insert(std::move(n));
}

bool PropertyGraph::AddEdge(UUID start, UUID end, UUID mid) { return false; }
bool PropertyGraph::AddEdge(Handle start, Handle end, UUID mid) {
  return false;
}
void PropertyGraph::RemoveNode(Handle node_handle) {}
void PropertyGraph::RemoveNode(UUID id) {}
void PropertyGraph::RemoveEdge(Handle edge_handle) {}
void PropertyGraph::RemoveEdge(UUID id) {}

}  // namespace rtpg