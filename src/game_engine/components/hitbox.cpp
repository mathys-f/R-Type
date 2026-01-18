#include "components/hitbox.h"
#include <cstring>

using namespace engn::cpnt;

Hitbox::Hitbox(float width, float height, float offset_x, float offset_y)
    : width(width), height(height), offset_x(offset_x), offset_y(offset_y) {}

engn::SerializedComponent Hitbox::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::hitbox;
    std::uint32_t size = sizeof(width) + sizeof(height) + sizeof(offset_x) + sizeof(offset_y);
    serialized.data.resize(size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &width, sizeof(width));
    offset += sizeof(width);
    std::memcpy(serialized.data.data() + offset, &height, sizeof(height));
    offset += sizeof(height);
    std::memcpy(serialized.data.data() + offset, &offset_x, sizeof(offset_x));
    offset += sizeof(offset_x);
    std::memcpy(serialized.data.data() + offset, &offset_y, sizeof(offset_y));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Hitbox::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(width) + sizeof(height) + sizeof(offset_x) + sizeof(offset_y);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&width, data.data() + offset, sizeof(width));
        offset += sizeof(width);
        std::memcpy(&height, data.data() + offset, sizeof(height));
        offset += sizeof(height);
        std::memcpy(&offset_x, data.data() + offset, sizeof(offset_x));
        offset += sizeof(offset_x);
        std::memcpy(&offset_y, data.data() + offset, sizeof(offset_y));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}