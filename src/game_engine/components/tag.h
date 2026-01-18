#pragma once

#include "ecs/tag_registry.h"
#include "components/i_sync_component.h"

namespace engn::cpnt {

/// Has two different uses
/// 1. UI, identifies an element
/// 2. Archetypes, for entities sent by the server, is related to a set of graphics only component
struct Tag : ISyncComponent {
    ecs::TagRegistry::TagId id = ecs::TagRegistry::k_invalid_tag_id;

    Tag() = default;
    Tag(ecs::TagRegistry::TagId id);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
