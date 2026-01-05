#pragma once

#include "ecs/tag_registry.h"
#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Tag : ISyncComponent {
    ecs::TagRegistry::TagId id = ecs::TagRegistry::k_invalid_tag_id;

    Tag() = default;
    Tag(ecs::TagRegistry::TagId id);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
