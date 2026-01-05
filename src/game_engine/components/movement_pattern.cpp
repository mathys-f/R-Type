#include "components/movement_pattern.h"
#include <cstring>

using namespace engn::cpnt;

MovementPattern::MovementPattern(PatternType type, float speed, float amplitude,
                               float frequency, float timer, float base_y)
    : type(type), speed(speed), amplitude(amplitude),
      frequency(frequency), timer(timer), base_y(base_y) {}

engn::SerializedComponent MovementPattern::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::movement_pattern;
    serialized.size = sizeof(type) + sizeof(speed) + sizeof(amplitude) + sizeof(frequency) + sizeof(timer) + sizeof(base_y);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &type, sizeof(type));
    offset += sizeof(type);
    std::memcpy(serialized.data.data() + offset, &speed, sizeof(speed));
    offset += sizeof(speed);
    std::memcpy(serialized.data.data() + offset, &amplitude, sizeof(amplitude));
    offset += sizeof(amplitude);
    std::memcpy(serialized.data.data() + offset, &frequency, sizeof(frequency));
    offset += sizeof(frequency);
    std::memcpy(serialized.data.data() + offset, &timer, sizeof(timer));
    offset += sizeof(timer);
    std::memcpy(serialized.data.data() + offset, &base_y, sizeof(base_y));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void MovementPattern::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(type) + sizeof(speed) + sizeof(amplitude) + sizeof(frequency) + sizeof(timer) + sizeof(base_y);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&type, data.data() + offset, sizeof(type));
        offset += sizeof(type);
        std::memcpy(&speed, data.data() + offset, sizeof(speed));
        offset += sizeof(speed);
        std::memcpy(&amplitude, data.data() + offset, sizeof(amplitude));
        offset += sizeof(amplitude);
        std::memcpy(&frequency, data.data() + offset, sizeof(frequency));
        offset += sizeof(frequency);
        std::memcpy(&timer, data.data() + offset, sizeof(timer));
        offset += sizeof(timer);
        std::memcpy(&base_y, data.data() + offset, sizeof(base_y));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}