#include "components/tag.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Tag::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::tag;
    serialized.size = sizeof(id);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &id, sizeof(id));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Tag::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(id);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&id, data.data() + offset, sizeof(id));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}