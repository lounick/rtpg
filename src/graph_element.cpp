
#include "rt-pg/graph_element.h"

namespace rtpg {
GraphElement::GraphElement(UUID id) : GraphElement(id, {}) {}
GraphElement::GraphElement(UUID id, const Properties& properties)
    : id_{id}, properties_{properties} {}
GraphElement::~GraphElement() {}
const UUID& GraphElement::id() const { return id_; }
const Properties& GraphElement::properties() const { return properties_; }
Properties& GraphElement::properties() { return properties_; }
}  // namespace rtpg
