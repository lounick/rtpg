#pragma once

#include <map>
#include <stack>
#include <vector>

#include "rt-pg/edge.h"
#include "rt-pg/handle.h"
#include "rt-pg/node.h"
#include "rt-pg/types.h"

namespace rtpg {
class PropertyGraph {
 public:
  PropertyGraph();
  PropertyGraph(std::size_t init_size);

  /**
   * @brief Adds a vertex to the graph.
   *
   * @param mids The model IDs the vertex confroms to
   * @return Handle The handle for accessing the vertex.
   */
  Handle addVertex(std::vector<UUID> mids, Properties properites);

  /**
   * @brief Adds an edge between two vertices in the graph using the UUIDs of
   * the vertices.
   *
   * @param start The UUID of the starting vertex for the edge.
   * @param end The UUID of the ending vertex for the edge.
   * @param mid The model ID of the edge.
   * @return true If the edge was added correctly.
   * @return false If the edge was not added.
   */
  bool addEdge(UUID start, UUID end, UUID mid);

  /**
   * @brief Adds an edge between two vertices in the graph using the handles of
   * the vertices.
   *
   * @param start The handle that describes the start vertex.
   * @param end The handle that describes the end vertex.
   * @param mid The model ID of the edge.
   * @return true If the edge was added correctly.
   * @return false If the edge was not added.
   */
  bool addEdge(Handle start, Handle end, UUID mid);

  void removeVertex(Handle vertex_handle);
  void removeVertex(UUID id);
  void removeEdge(Handle edge_handle);
  void removeEdge(UUID id);

 private:
  bool addFreeNodes(std::size_t additional_nodes);
  bool addFreeEdges(std::size_t additional_edges);
  std::vector<NodeItem> nodes_;
  std::vector<EdgeItem> edges_;
  std::map<UUID, std::vector<Handle>> mids_;
  std::stack<std::size_t> free_nodes_;
  std::stack<std::size_t> free_edges_;
};

}  // namespace rtpg
