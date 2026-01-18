#pragma once

#include <vector>
#include <cstddef>

#include "snapshots.h"

namespace engn::cpnt {

// All components inheriting from ISyncComponent can be serialized and deserialized
// for synchronization purposes (e.g., networked multiplayer).
struct ISyncComponent {
    virtual ~ISyncComponent() = default;

    ISyncComponent() = default;
    ISyncComponent(const ISyncComponent&) = default;
    ISyncComponent(ISyncComponent&&) = default;
    ISyncComponent& operator=(const ISyncComponent&) = default;
    ISyncComponent& operator=(ISyncComponent&&) = default;

    virtual engn::SerializedComponent serialize() const = 0;
    virtual void deserialize(const std::vector<std::byte>& data) = 0;
};

} // namespace engn::cpnt
