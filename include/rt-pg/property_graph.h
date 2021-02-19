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

  std::optional<Handle> AddNode(UUID id, std::vector<UUID> mids,
                                Properties properites);

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
  std::optional<Handle> AddEdge(UUID start, UUID end, UUID mid);

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
  std::optional<Handle> AddEdge(Handle start, Handle end, UUID mid);

  bool RemoveNode(UUID id);
  bool RemoveNode(Handle node_handle);
  bool RemoveEdge(UUID id);
  bool RemoveEdge(Handle edge_handle);

  // TODO: Add querrying for specific nodes or edges

 private:
  void RollbackAddNode(const Handle& handle,
                       const std::map<UUID, Handle>::iterator& it,
                       const std::vector<UUID>& mids);

  std::shared_ptr<Edge> GetEdgePtrFromHandle(const Handle& edge_handle);
  std::shared_ptr<const Edge> GetConstEdgePtrFromHandle(
      const Handle& edge_handle);
  std::shared_ptr<Node> GetNodePtrFromHandle(const Handle& node_handle);
  std::shared_ptr<const Node> GetConstNodePtrFromHandle(
      const Handle& node_handle);

  void RemoveHandleFromEdgeList(Handle& h, std::vector<Handle>& list);

  handle_allocator::HandleAllocator<Node> nodes_;
  handle_allocator::HandleAllocator<Edge> edges_;
  std::map<UUID, std::vector<Handle>> mids_;
  std::map<UUID, Handle> uuid_to_handle_;
  std::unique_ptr<uuids::uuid_random_generator> uuidv1_generator_;
  std::unique_ptr<std::mt19937> gen_;
};

}  // namespace rtpg
