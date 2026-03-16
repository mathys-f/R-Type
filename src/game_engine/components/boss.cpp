#include "components/boss.h"

#include <cstring>

using namespace engn::cpnt;

Boss::Boss(float time, float cooldown_1, float cooldown_2, bool time_to_roar, bool roar_active, Vector2 center,
           float radius, float speed)
    : timer(time), cooldown_1(cooldown_1), cooldown_2(cooldown_2), time_to_roar(time_to_roar), roar_active(roar_active),
      wave_center(center), wave_radius(radius), wave_speed(speed) {}

engn::SerializedComponent Boss::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::Boss;
    const std::uint16_t k_total_size = sizeof(timer) + sizeof(cooldown_1) + sizeof(cooldown_2) + sizeof(time_to_roar) +
                                       sizeof(roar_active) + sizeof(wave_center) + sizeof(wave_radius) +
                                       sizeof(wave_speed);
    serialized.data.resize(k_total_size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &timer, sizeof(timer));
    offset += sizeof(timer);
    std::memcpy(serialized.data.data() + offset, &cooldown_1, sizeof(cooldown_1));
    offset += sizeof(cooldown_1);
    std::memcpy(serialized.data.data() + offset, &cooldown_2, sizeof(cooldown_2));
    offset += sizeof(cooldown_2);
    std::memcpy(serialized.data.data() + offset, &time_to_roar, sizeof(time_to_roar));
    offset += sizeof(time_to_roar);
    std::memcpy(serialized.data.data() + offset, &roar_active, sizeof(roar_active));
    offset += sizeof(roar_active);
    std::memcpy(serialized.data.data() + offset, &wave_center, sizeof(wave_center));
    offset += sizeof(wave_center);
    std::memcpy(serialized.data.data() + offset, &wave_radius, sizeof(wave_radius));
    offset += sizeof(wave_radius);
    std::memcpy(serialized.data.data() + offset, &wave_speed, sizeof(wave_speed));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Boss::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(timer) + sizeof(cooldown_1) + sizeof(cooldown_2) + sizeof(time_to_roar) +
                         sizeof(roar_active) + sizeof(wave_center) + sizeof(wave_radius) + sizeof(wave_speed);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&timer, data.data() + offset, sizeof(timer));
        offset += sizeof(timer);
        std::memcpy(&cooldown_1, data.data() + offset, sizeof(cooldown_1));
        offset += sizeof(cooldown_1);
        std::memcpy(&cooldown_2, data.data() + offset, sizeof(cooldown_2));
        offset += sizeof(cooldown_2);
        std::memcpy(&time_to_roar, data.data() + offset, sizeof(time_to_roar));
        offset += sizeof(time_to_roar);
        std::memcpy(&roar_active, data.data() + offset, sizeof(roar_active));
        offset += sizeof(roar_active);
        std::memcpy(&wave_center, data.data() + offset, sizeof(wave_center));
        offset += sizeof(wave_center);
        std::memcpy(&wave_radius, data.data() + offset, sizeof(wave_radius));
        offset += sizeof(wave_radius);
        std::memcpy(&wave_speed, data.data() + offset, sizeof(wave_speed));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}
