#pragma once

#include <handle_allocator/handle.h>
#include <handle_allocator/handle_allocator.h>

#include <functional>
#include <map>
#include <optional>
#include <random>
#include <vector>

#include "rt-pg/edge.h"
#include "rt-pg/node.h"
#include "rt-pg/types.h"

namespace rtpg {
class PropertyGraph {
 public:
  PropertyGraph();

  /**
   * @brief Adds a node to the graph.
   *
   * @param mids The model IDs the vertex confroms to
   * @return Handle The handle for accessing the vertex.
   */
  std::optional<Handle> AddNode(std::vector<UUID> mids, Properties properites);

  /**
   * @brief Adds an edge between two nodes in the graph using the UUIDs of
   * the nodes.
   *
   * @param start The UUID of the starting node for the edge.
   * @param end The UUID of the ending node for the edge.
   * @param mid The model ID of the edge.
   * @return true If the edge was added correctly.
   * @return false If the edge was not added.
   */
  bool AddEdge(UUID start, UUID end, UUID mid);

  /**
   * @brief Adds an edge between two nodes in the graph using the handles of
   * the nodes.
   *
   * @param start The handle that describes the start node.
   * @param end The handle that describes the end node.
   * @param mid The model ID of the edge.
   * @return true If the edge was added correctly.
   * @return false If the edge was not added.
   */
  bool AddEdge(Handle start, Handle end, UUID mid);

  void RemoveNode(Handle node_handle);
  void RemoveNode(UUID id);
  void RemoveEdge(Handle edge_handle);
  void RemoveEdge(UUID id);

 private:
  handle_allocator::HandleAllocator<Node> nodes_;
  handle_allocator::HandleAllocator<Edge> edges_;
  std::map<UUID, std::vector<Handle>> mids_;
  std::unique_ptr<uuids::uuid_random_generator> uuidv1_generator_;
};

}  // namespace rtpg
