#include "rt-pg/property_graph.h"

#include <iostream>

namespace rtpg {
PropertyGraph::PropertyGraph() : PropertyGraph(256) {}

PropertyGraph::PropertyGraph(std::size_t init_size) {
  addFreeNodes(init_size);
  addFreeEdges(init_size);
}

bool PropertyGraph::addFreeNodes(std::size_t additional_nodes) {
  std::size_t current_nodes = nodes_.size();
  std::size_t final_nodes = current_nodes + additional_nodes;
  try {
    nodes_.resize(final_nodes);
  } catch (std::bad_alloc &e) {
    std::cerr << "Allocation of new nodes failed: " << e.what() << '\n';
    return false;
  }
  for (std::size_t idx = current_nodes; idx < final_nodes; ++idx) {
    nodes_.at(idx).handle.index = idx;
    nodes_.at(idx).handle.unique_pattern = 1;
    free_nodes_.push(idx);
  }
  return true;
}

bool PropertyGraph::addFreeEdges(std::size_t additional_edges) {
  std::size_t current_edges = edges_.size();
  std::size_t final_edges = current_edges + additional_edges;
  try {
    edges_.resize(final_edges);
  } catch (std::bad_alloc &e) {
    std::cerr << "Allocation of new edges failed: " << e.what() << '\n';
    return false;
  }
  for (std::size_t idx = current_edges; idx < final_edges; ++idx) {
    edges_.at(idx).handle.index = idx;
    edges_.at(idx).handle.unique_pattern = 1;
    free_edges_.push(idx);
  }
  return true;
}

// TODO: this thould return an optional asit maybe not be able to add it
Handle PropertyGraph::addVertex(std::vector<UUID> mds, Properties properties) {
  std::size_t pos;
  if (free_nodes_.empty()) {  // Add extra capacity
  }
}
// TODO: this thould return an optional s it maybe not be able to add it
bool PropertyGraph::addEdge(UUID start, UUID end, UUID mid) {}
// TODO: this thould return an optional as it maybe not be able to add it
bool PropertyGraph::addEdge(Handle start, Handle end, UUID mid) {}

void PropertyGraph::removeVertex(Handle vertex_handle) {}
void PropertyGraph::removeVertex(UUID id) {}
void PropertyGraph::removeEdge(Handle edge_handle) {}
void PropertyGraph::removeEdge(UUID id) {}

}  // namespace rtpg
