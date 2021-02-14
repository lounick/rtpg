#include "rt-pg/property_graph.h"

#include <iostream>

namespace rtpg {
PropertyGraph::PropertyGraph() {
  std::random_device rd;
  gen_.reset(new std::mt19937(rd()));
  uuidv1_generator_.reset(new uuids::uuid_random_generator(gen_.get()));
}

// TODO: This whole thing shoud be considered atomic operation for
// multi-threading. So it should be appropriatelly mutexed eventually.
// Or should have some memory manager that keeps track the available memory
// and we should calculate how much would we need for the operation and fail
// imediately if that would not be available. That would prevent any rollbacks.
std::optional<Handle> PropertyGraph::AddNode(std::vector<UUID> mids,
                                             Properties properites) {
  UUID id = (*uuidv1_generator_)();
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
      auto [it, success] = uuid_to_handle_.insert({id, ret.value()});
      uuid_to_handle_it = it;
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
          auto [it, success] = mids_.insert({mid, std::vector<Handle>()});
          it->second.push_back(ret.value());
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

  return AddEdge(start_handle, end_handle, mid);
}
std::optional<Handle> PropertyGraph::AddEdge(Handle start, Handle end,
                                             UUID mid) {
  std::shared_ptr<Node> start_node_ptr;
  std::shared_ptr<Node> end_node_ptr;
  try {
    start_node_ptr = nodes_.Get(start).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cout << e.what() << '\n';
    return {};
  }

  try {
    end_node_ptr = nodes_.Get(end).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cout << e.what() << '\n';
    return {};
  }

  // TODO: This may not be needed.
  if (start_node_ptr == nullptr || end_node_ptr == nullptr) {
    return {};
  }

  UUID id = (*uuidv1_generator_)();
  Edge e(id, mid);
  e.head(end);
  e.tail(start);
  std::optional<Handle> ret = edges_.Insert(std::move(e));
  try {
    Handle edge_handle = ret.value();
    try {
      start_node_ptr->out_edges().push_back(edge_handle);
    } catch (const std::exception& e) {
      std::cout << e.what() << '\n';
      edges_.Delete(edge_handle);
      return {};
    }
    try {
      end_node_ptr->in_edges().push_back(edge_handle);
    } catch (const std::exception& e) {
      std::cout << e.what() << '\n';
      start_node_ptr->out_edges().pop_back();
      edges_.Delete(edge_handle);
      return {};
    }

    try {
      uuid_to_handle_.insert({id, edge_handle});
    } catch (const std::exception& e) {
      std::cout << e.what() << '\n';
      start_node_ptr->out_edges().pop_back();
      end_node_ptr->in_edges().pop_back();
      edges_.Delete(edge_handle);
      return {};
    }
  } catch (const std::bad_optional_access& e) {
    std::cout << e.what() << '\n';
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

bool PropertyGraph::RemoveNode(Handle node_handle) {
  // TODO
  // For each outgoing edge
  //// Traverse it and find the incomming node
  //// Remove edge from that node
  //// Remove this edge
  // Remove the node
  std::shared_ptr<Node> node_ptr;
  return false;
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

bool PropertyGraph::RemoveEdge(Handle edge_handle) {
  std::shared_ptr<const Edge> edge_ptr;
  try {
    edge_ptr = edges_.GetConst(edge_handle).value().lock();
  } catch (const std::bad_optional_access& e) {
    std::cout << e.what() << '\n';
    return false;
  }

  const Handle start = edge_ptr->tail();
  const Handle end = edge_ptr->head();

  std::shared_ptr<Node> start_node_ptr;
  std::shared_ptr<Node> end_node_ptr;
  try {
    start_node_ptr = nodes_.Get(start).value().lock();
    std::vector<Handle>& out_edges = start_node_ptr->out_edges();
    out_edges.erase(
        std::remove(out_edges.begin(), out_edges.end(), edge_handle),
        out_edges.end());
  } catch (const std::bad_optional_access& e) {
    std::cout << e.what() << '\n';
  }

  try {
    end_node_ptr = nodes_.Get(end).value().lock();
    std::vector<Handle>& in_edges = end_node_ptr->in_edges();
    in_edges.erase(std::remove(in_edges.begin(), in_edges.end(), edge_handle),
                   in_edges.end());
  } catch (const std::bad_optional_access& e) {
    std::cout << e.what() << '\n';
  }

  uuid_to_handle_.erase(edge_ptr->id());
  return edges_.Delete(edge_handle);
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

}  // namespace rtpg