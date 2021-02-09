#pragma once

#include <bitset>
#include <vector>

#include "handle_allocator/handle.h"
#include "rt-pg/property.h"
#include "uuid.h"

namespace rtpg {
using UUID = uuids::uuid;
using Handle = handle_allocator::Handle;
using Properties = std::vector<Property>;
}  // namespace rtpg
