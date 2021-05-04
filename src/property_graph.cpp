#include "rt-pg/property_graph.h"

#include <iostream>

namespace rtpg {
PropertyGraph::PropertyGraph() {
  std::random_device rd;
  gen_.reset(new std::mt19937(rd()));
  uuidv1_generator_.reset(new uuids::uuid_random_generator(gen_.get()));
}

std::optional<Handle> PropertyGraph::AddNode(std::vector<UUID> mids) {
  return AddNode(mids, {});
}

std::optional<Handle> PropertyGraph::AddNode(UUID id, std::vector<UUID> mids) {
  return AddNode(id, mids, {});
}

std::optional<Handle> PropertyGraph::AddNode(std::vector<UUID> mids,
                                             const Properties& properites) {
  UUID id = (*uuidv1_generator_)();
  return AddNode(id, mids, properites);
}

// TODO: This whole thing shoud be considered atomic operation for
// multi-threading. So it should be appropriatelly mutexed eventually.
// Or should have some memory manager that keeps track the available memory
// and we should calculate how much would we need for the operation and fail
// imediately if that would not be available. That would prevent any rollbacks.
std::optional<Handle> PropertyGraph::AddNode(UUID id, std::vector<UUID> mids,
                                             const Properties& properites) {
  Node n(id, mids, properites);
  std::optional<Handle> ret = nodes_.Insert(std::move(n));
  std::vector<UUID> successful_mids;
  if (ret.has_value()) {
    // Initialise variables
    successful_mids.clear();
    std::map<UUID, Handle>::iterator uuid_to_handle_it = uuid_to_handle_.end();

    // Try to allocate memory for mid bookkeeping
    try {
      successful_mids.reserve(mids.size());
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      RollbackAddNode(ret.value(), uuid_to_handle_it, successful_mids);
      return {};
    }

    // Try to map the UUID to handle
    try {
      auto ins_ret = uuid_to_handle_.insert({id, ret.value()});
      uuid_to_handle_it = ins_ret.first;
      // Something with this UUID exists. Just return
      // TODO: Maybe throw or print something
      if (!ins_ret.second) {
        nodes_.Delete(ret.value());
        return {};
      }
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      RollbackAddNode(ret.value(), uuid_to_handle_it, successful_mids);
      return {};
    }

    for (const UUID& mid : mids) {
      try {
        // Try to insert normaly
        mids_.at(mid).push_back(ret.value());
      } catch (const std::out_of_range& e) {
        // If there is no mid key present
        try {
          // Try to create one and insert
          auto ins_ret = mids_.insert({mid, std::vector<Handle>()});
          // Iterator is first, second is the value of the K,V pair
          ins_ret.first->second.push_back(ret.value());
        } catch (const std::exception& ex) {
          // If any of these fails rollback and fail adding
          std::cerr << e.what() << '\n';
          RollbackAddNode(ret.value(), uuid_to_handle_it, successful_mids);
          return {};
        }
      } catch (const std::exception& e) {
        // If something else happens just rollback any changes and fail adding
        std::cerr << e.what() << '\n';
        RollbackAddNode(ret.value(), uuid_to_handle_it, successful_mids);
        return {};
      }
      successful_mids.push_back(mid);
    }
  }
  return ret;
}

std::optional<Handle> PropertyGraph::AddEdge(UUID start, UUID end, UUID mid) {
  return AddEdge(start, end, mid, {});
}

std::optional<Handle> PropertyGraph::AddEdge(const Handle& start,
                                             const Handle& end, UUID mid) {
  return AddEdge(start, end, mid, {});
}

std::optional<Handle> PropertyGraph::AddEdge(UUID start, UUID end, UUID mid,
                                             const Properties& properties) {
  // If UUIDs are the same return
  // TODO: Maybe throw an error
  if (start == end) {
    return {};
  }

  Handle start_handle, end_handle;
  // Try to get Handles from UUIDs. Fail if one of the two is not there.
  try {
    start_handle = uuid_to_handle_.at(start);
  } catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
    return {};
  }

  try {
    end_handle = uuid_to_handle_.at(end);
  } catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
    return {};
  }

  return AddEdge(start_handle, end_handle, mid, properties);
}
std::optional<Handle> PropertyGraph::AddEdge(const Handle& start,
                                             const Handle& end, UUID mid,
                                             const Properties& properties) {
  // If handles are the same return
  // TODO: Maybe throw an error
  if (start == end) {
    return {};
  }

  std::shared_ptr<Node> start_node_ptr;
  std::shared_ptr<Node> end_node_ptr;
  try {
    start_node_ptr = nodes_.Get(start).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cerr << e.what() << '\n';
    return {};
  }

  try {
    end_node_ptr = nodes_.Get(end).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cerr << e.what() << '\n';
    return {};
  }

  // TODO: This may not be needed.
  if (start_node_ptr == nullptr || end_node_ptr == nullptr) {
    return {};
  }

  UUID id = (*uuidv1_generator_)();
  Edge e(id, mid, end, start, properties);
  std::optional<Handle> ret = edges_.Insert(std::move(e));
  try {
    Handle edge_handle = ret.value();
    try {
      start_node_ptr->out_edges().push_back(edge_handle);
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      edges_.Delete(edge_handle);
      return {};
    }
    try {
      end_node_ptr->in_edges().push_back(edge_handle);
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      start_node_ptr->out_edges().pop_back();
      edges_.Delete(edge_handle);
      return {};
    }

    try {
      uuid_to_handle_.insert({id, edge_handle});
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      start_node_ptr->out_edges().pop_back();
      end_node_ptr->in_edges().pop_back();
      edges_.Delete(edge_handle);
      return {};
    }
  } catch (const std::bad_optional_access& e) {
    std::cerr << e.what() << '\n';
    return {};
  }
  return ret;
}

bool PropertyGraph::RemoveNode(UUID id) {
  Handle node_handle;
  try {
    node_handle = uuid_to_handle_.at(id);
  } catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
    return false;
  }
  return RemoveNode(node_handle);
}

bool PropertyGraph::RemoveNode(const Handle& node_handle) {
  std::shared_ptr<Node> node_ptr = GetNodePtrFromHandle(node_handle);
  if (node_ptr == nullptr) {
    return false;
  }

  std::vector<Handle>& out_edges = node_ptr->out_edges();
  for (Handle& edge_handle : out_edges) {
    std::shared_ptr<Edge> edge_ptr = GetEdgePtrFromHandle(edge_handle);
    // TODO: Check if these pointer checks make sense
    if (edge_ptr != nullptr) {
      const Handle end = edge_ptr->head();
      std::shared_ptr<Node> end_node_ptr = GetNodePtrFromHandle(end);
      if (end_node_ptr != nullptr) {
        RemoveHandleFromEdgeList(edge_handle, end_node_ptr->in_edges());
      }
    }
    edges_.Delete(edge_handle);
  }

  return nodes_.Delete(node_handle);
}

bool PropertyGraph::RemoveEdge(UUID id) {
  Handle edge_handle;
  try {
    edge_handle = uuid_to_handle_.at(id);
  } catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
    return false;
  }
  return RemoveEdge(edge_handle);
}

bool PropertyGraph::RemoveEdge(const Handle& edge_handle) {
  std::shared_ptr<const Edge> edge_ptr = GetConstEdgePtrFromHandle(edge_handle);

  if (edge_ptr == nullptr) {
    return false;
  }

  const Handle start = edge_ptr->tail();
  const Handle end = edge_ptr->head();

  std::shared_ptr<Node> start_node_ptr = GetNodePtrFromHandle(start);
  if (start_node_ptr != nullptr) {
    std::vector<Handle>& out_edges = start_node_ptr->out_edges();
    RemoveHandleFromEdgeList(edge_handle, out_edges);
  }

  std::shared_ptr<Node> end_node_ptr = GetNodePtrFromHandle(end);
  if (end_node_ptr != nullptr) {
    std::vector<Handle>& in_edges = end_node_ptr->in_edges();
    RemoveHandleFromEdgeList(edge_handle, in_edges);
  }

  uuid_to_handle_.erase(edge_ptr->id());
  return edges_.Delete(edge_handle);
}

ConstNodePtr PropertyGraph::GetConstNodePtr(UUID id) {
  Handle handle;
  ConstNodePtr ptr = nullptr;
  try {
    handle = uuid_to_handle_.at(id);
    ptr = GetConstNodePtrFromHandle(handle);
  } catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
  }
  return ptr;
}

NodePtr PropertyGraph::GetNodePtr(UUID id) {
  Handle handle;
  NodePtr ptr = nullptr;
  try {
    handle = uuid_to_handle_.at(id);
    ptr = GetNodePtrFromHandle(handle);
  } catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
  }
  return ptr;
}

ConstNodePtr PropertyGraph::GetConstNodePtr(const Handle& handle) {
  return GetConstNodePtrFromHandle(handle);
}

NodePtr PropertyGraph::GetNodePtr(const Handle& handle) {
  return GetNodePtrFromHandle(handle);
}

ConstEdgePtr PropertyGraph::GetConstEdgePtr(UUID id) {
  Handle handle;
  ConstEdgePtr ptr;
  try {
    handle = uuid_to_handle_.at(id);
    ptr = GetConstEdgePtrFromHandle(handle);
  } catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
  }
  return ptr;
}

EdgePtr PropertyGraph::GetEdgePtr(UUID id) {
  Handle handle;
  EdgePtr ptr;
  try {
    handle = uuid_to_handle_.at(id);
    ptr = GetEdgePtrFromHandle(handle);
  } catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
  }
  return ptr;
}

ConstEdgePtr PropertyGraph::GetConstEdgePtr(const Handle& handle) {
  return GetConstEdgePtrFromHandle(handle);
}

EdgePtr PropertyGraph::GetEdgePtr(const Handle& handle) {
  return GetEdgePtrFromHandle(handle);
}

void PropertyGraph::RollbackAddNode(const Handle& handle,
                                    const std::map<UUID, Handle>::iterator& it,
                                    const std::vector<UUID>& mids) {
  for (const UUID& mid : mids) {
    mids_.at(mid).pop_back();
  }
  if (it != uuid_to_handle_.end()) {
    uuid_to_handle_.erase(it);
  }
  nodes_.Delete(handle);
}

EdgePtr PropertyGraph::GetEdgePtrFromHandle(const Handle& edge_handle) {
  EdgePtr edge_ptr = nullptr;
  try {
    edge_ptr = edges_.Get(edge_handle).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cerr << e.what() << '\n';
  }
  return edge_ptr;
}

ConstEdgePtr PropertyGraph::GetConstEdgePtrFromHandle(
    const Handle& edge_handle) {
  ConstEdgePtr edge_ptr = nullptr;
  try {
    edge_ptr = edges_.GetConst(edge_handle).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cerr << e.what() << '\n';
  }
  return edge_ptr;
}

NodePtr PropertyGraph::GetNodePtrFromHandle(const Handle& node_handle) {
  NodePtr node_ptr = nullptr;
  try {
    node_ptr = nodes_.Get(node_handle).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cerr << e.what() << '\n';
  }
  return node_ptr;
}

ConstNodePtr PropertyGraph::GetConstNodePtrFromHandle(
    const Handle& node_handle) {
  ConstNodePtr node_ptr = nullptr;
  try {
    node_ptr = nodes_.GetConst(node_handle).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cerr << e.what() << '\n';
  }
  return node_ptr;
}

void PropertyGraph::RemoveHandleFromEdgeList(const Handle& h,
                                             std::vector<Handle>& list) {
  list.erase(std::remove(list.begin(), list.end(), h), list.end());
}

}  // namespace rtpg
