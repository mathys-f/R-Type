#pragma once

#include <vector>
#include <cstddef>

#include "snapshots.h"

namespace engn::cpnt {

struct ISyncComponent {
    virtual ~ISyncComponent() = default;
    virtual engn::SerializedComponent serialize() const = 0;
    virtual void deserialize(const std::vector<std::byte>& data) = 0;
};

} // namespace engn::cpnt
