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
  successful_mids.reserve(mids.size());
  if (ret.has_value()) {
    uuid_to_handle_.insert({id, ret.value()});
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
          RollbackAddNode(ret.value(), successful_mids);
          return {};
        }
      } catch (const std::exception& e) {
        // If something else happens just rollback any changes and fail adding
        std::cerr << e.what() << '\n';
        RollbackAddNode(ret.value(), successful_mids);
        return {};
      }
      successful_mids.push_back(mid);
    }
  }
  return ret;
}

std::optional<Handle> PropertyGraph::AddEdge(UUID start, UUID end, UUID mid) {
  return {};
}
std::optional<Handle> PropertyGraph::AddEdge(Handle start, Handle end,
                                             UUID mid) {
  return {};
}
void PropertyGraph::RemoveNode(Handle node_handle) {}
void PropertyGraph::RemoveNode(UUID id) {}
void PropertyGraph::RemoveEdge(Handle edge_handle) {}
void PropertyGraph::RemoveEdge(UUID id) {}

void PropertyGraph::RollbackAddNode(const Handle& handle,
                                    const std::vector<UUID>& mids) {
  for (const UUID& mid : mids) {
    mids_.at(mid).pop_back();
  }
  nodes_.Delete(handle);
}

}  // namespace rtpg