#include "components/boss_hitbox.h"
#include <cstring>

using namespace engn::cpnt;

BossHitbox::BossHitbox(float width_1, float height_1, float offset_x_1, float offset_y_1, 
                       float width_2, float height_2, float offset_x_2, float offset_y_2,
                       float width_3, float height_3, float offset_x_3, float offset_y_3)
    : width_1(width_1), height_1(height_1), offset_x_1(offset_x_1), offset_y_1(offset_y_1),
      width_2(width_2), height_2(height_2), offset_x_2(offset_x_2), offset_y_2(offset_y_2),
      width_3(width_3), height_3(height_3), offset_x_3(offset_x_3), offset_y_3(offset_y_3) {}

engn::SerializedComponent BossHitbox::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::boss_hitbox;
    const std::uint16_t k_total_size = sizeof(width_1) + sizeof(height_1) + sizeof(offset_x_1) + sizeof(offset_y_1) +
                      sizeof(width_2) + sizeof(height_2) + sizeof(offset_x_2) + sizeof(offset_y_2) +
                      sizeof(width_3) + sizeof(height_3) + sizeof(offset_x_3) + sizeof(offset_y_3);
    serialized.data.resize(k_total_size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &width_1, sizeof(width_1));
    offset += sizeof(width_1);
    std::memcpy(serialized.data.data() + offset, &height_1, sizeof(height_1));
    offset += sizeof(height_1);
    std::memcpy(serialized.data.data() + offset, &offset_x_1, sizeof(offset_x_1));
    offset += sizeof(offset_x_1);
    std::memcpy(serialized.data.data() + offset, &offset_y_1, sizeof(offset_y_1));
    offset += sizeof(offset_y_1);
    std::memcpy(serialized.data.data() + offset, &width_2, sizeof(width_2));
    offset += sizeof(width_2);
    std::memcpy(serialized.data.data() + offset, &height_2, sizeof(height_2));
    offset += sizeof(height_2);
    std::memcpy(serialized.data.data() + offset, &offset_x_2, sizeof(offset_x_2));
    offset += sizeof(offset_x_2);
    std::memcpy(serialized.data.data() + offset, &offset_y_2, sizeof(offset_y_2));
    offset += sizeof(offset_y_2);
    std::memcpy(serialized.data.data() + offset, &width_3, sizeof(width_3));
    offset += sizeof(width_3);
    std::memcpy(serialized.data.data() + offset, &height_3, sizeof(height_3));
    offset += sizeof(height_3);
    std::memcpy(serialized.data.data() + offset, &offset_x_3, sizeof(offset_x_3));
    offset += sizeof(offset_x_3);
    std::memcpy(serialized.data.data() + offset, &offset_y_3, sizeof(offset_y_3));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void BossHitbox::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(width_1) + sizeof(height_1) + sizeof(offset_x_1) + sizeof(offset_y_1) +
                         sizeof(width_2) + sizeof(height_2) + sizeof(offset_x_2) + sizeof(offset_y_2) +
                         sizeof(width_3) + sizeof(height_3) + sizeof(offset_x_3) + sizeof(offset_y_3);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&width_1, data.data() + offset, sizeof(width_1));
        offset += sizeof(width_1);
        std::memcpy(&height_1, data.data() + offset, sizeof(height_1));
        offset += sizeof(height_1);
        std::memcpy(&offset_x_1, data.data() + offset, sizeof(offset_x_1));
        offset += sizeof(offset_x_1);
        std::memcpy(&offset_y_1, data.data() + offset, sizeof(offset_y_1));
        offset += sizeof(offset_y_1);
        std::memcpy(&width_2, data.data() + offset, sizeof(width_2));
        offset += sizeof(width_2);
        std::memcpy(&height_2, data.data() + offset, sizeof(height_2));
        offset += sizeof(height_2);
        std::memcpy(&offset_x_2, data.data() + offset, sizeof(offset_x_2));
        offset += sizeof(offset_x_2);
        std::memcpy(&offset_y_2, data.data() + offset, sizeof(offset_y_2));
        offset += sizeof(offset_y_2);
        std::memcpy(&width_3, data.data() + offset, sizeof(width_3));
        offset += sizeof(width_3);
        std::memcpy(&height_3, data.data() + offset, sizeof(height_3));
        offset += sizeof(height_3);
        std::memcpy(&offset_x_3, data.data() + offset, sizeof(offset_x_3));
        offset += sizeof(offset_x_3);
        std::memcpy(&offset_y_3, data.data() + offset, sizeof(offset_y_3));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}