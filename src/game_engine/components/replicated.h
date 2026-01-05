#pragma once

#include <string>

#include "components/i_sync_component.h"

namespace engn::cpnt {

// Component that marks an entity as replicated over the network.
struct Replicated : ISyncComponent {
    u_int32_t tag;

    size_t last_update_tick = 0;
    
    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
