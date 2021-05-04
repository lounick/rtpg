#pragma once

#include <bitset>
#include <map>
#include <string>
#include <vector>

#include "handle_allocator/handle.h"
#include "uuid.h"

namespace rtpg {
using UUID = uuids::uuid;
using Handle = handle_allocator::Handle;
using Property = std::variant<bool, int64_t, double, std::string>;
using Properties = std::map<std::string, Property>;
}  // namespace rtpg
