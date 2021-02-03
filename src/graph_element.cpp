#include "rt-pg/graph_element.h"

namespace rtpg {
GraphElement::GraphElement(UUID id) : id_(id) {}
const UUID& GraphElement::id() const { return id_; }
const Properties& GraphElement::properties() const { return properties_; }
}  // namespace rtpg
