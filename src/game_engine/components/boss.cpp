#include "components/boss.h"
#include <cstring>


using namespace engn::cpnt;

Boss::Boss(float time, bool roar_active,
           Vector2 center,
           float radius,
           float speed)
    : timer(time),
      roar_active(roar_active),
      waveCenter(center),
      waveRadius(radius),
      waveSpeed(speed) {}

engn::SerializedComponent Boss::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::boss;
    serialized.size = sizeof(timer) + sizeof(roar_active) + sizeof(waveCenter) + sizeof(waveRadius) + sizeof(waveSpeed);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &timer, sizeof(timer));
    offset += sizeof(timer);
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
    std::uint16_t size = sizeof(timer) + sizeof(roar_active) + sizeof(waveCenter) + sizeof(waveRadius) + sizeof(waveSpeed);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&timer, data.data() + offset, sizeof(timer));
        offset += sizeof(timer);
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
