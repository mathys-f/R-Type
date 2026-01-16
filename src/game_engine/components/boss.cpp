#include "components/boss.h"
#include <cstring>


using namespace engn::cpnt;

Boss::Boss(float time,
           float cooldown_1,
           float cooldown_2,
           bool time_to_roar,
           bool roar_active,
           Vector2 center,
           float radius,
           float speed)
    : timer(time),
      cooldown_1(cooldown_1),
      cooldown_2(cooldown_2),
      time_to_roar(time_to_roar),
      roar_active(roar_active),
      waveCenter(center),
      waveRadius(radius),
      waveSpeed(speed) {}

engn::SerializedComponent Boss::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::boss;
    serialized.size = sizeof(timer) + sizeof(cooldown_1) + sizeof(cooldown_2) + sizeof(time_to_roar) + sizeof(roar_active) + sizeof(waveCenter) + sizeof(waveRadius) + sizeof(waveSpeed);
    serialized.data.resize(serialized.size);

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
    std::memcpy(serialized.data.data() + offset, &waveCenter, sizeof(waveCenter));
    offset += sizeof(waveCenter);
    std::memcpy(serialized.data.data() + offset, &waveRadius, sizeof(waveRadius));
    offset += sizeof(waveRadius);
    std::memcpy(serialized.data.data() + offset, &waveSpeed, sizeof(waveSpeed));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Boss::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(timer) + sizeof(cooldown_1) + sizeof(cooldown_2) + sizeof(time_to_roar) + sizeof(roar_active) + sizeof(waveCenter) + sizeof(waveRadius) + sizeof(waveSpeed);

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
        std::memcpy(&waveCenter, data.data() + offset, sizeof(waveCenter));
        offset += sizeof(waveCenter);
        std::memcpy(&waveRadius, data.data() + offset, sizeof(waveRadius));
        offset += sizeof(waveRadius);
        std::memcpy(&waveSpeed, data.data() + offset, sizeof(waveSpeed));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}
