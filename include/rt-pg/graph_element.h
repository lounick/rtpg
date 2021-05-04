#pragma once

#include "rt-pg/property.h"
#include "rt-pg/types.h"

namespace rtpg {
class GraphElement {
 public:
  GraphElement(UUID id);
  GraphElement(UUID id, const Properties& properties);
  virtual ~GraphElement();
  const UUID& id() const;
  const Properties& properties() const;
  Properties& properties();

 protected:
  UUID id_;
  Properties properties_;
};
}  // namespace rtpg
