#include "components/explosion.h"
#include <cstring>

using namespace engn::cpnt;

Explosion::Explosion(ExplosionType type, float frame_duration, int total_frames,
                    float timer, int current_frame)
    : type(type), timer(timer), frame_duration(frame_duration),
      current_frame(current_frame), total_frames(total_frames) {}

engn::SerializedComponent Explosion::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::explosion;
    serialized.size = sizeof(type) + sizeof(timer) + sizeof(frame_duration) + sizeof(current_frame) + sizeof(total_frames);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &type, sizeof(type));
    offset += sizeof(type);
    std::memcpy(serialized.data.data() + offset, &timer, sizeof(timer));
    offset += sizeof(timer);
    std::memcpy(serialized.data.data() + offset, &frame_duration, sizeof(frame_duration));
    offset += sizeof(frame_duration);
    std::memcpy(serialized.data.data() + offset, &current_frame, sizeof(current_frame));
    offset += sizeof(current_frame);
    std::memcpy(serialized.data.data() + offset, &total_frames, sizeof(total_frames));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Explosion::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(type) + sizeof(timer) + sizeof(frame_duration) + sizeof(current_frame) + sizeof(total_frames);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&type, data.data() + offset, sizeof(type));
        offset += sizeof(type);
        std::memcpy(&timer, data.data() + offset, sizeof(timer));
        offset += sizeof(timer);
        std::memcpy(&frame_duration, data.data() + offset, sizeof(frame_duration));
        offset += sizeof(frame_duration);
        std::memcpy(&current_frame, data.data() + offset, sizeof(current_frame));
        offset += sizeof(current_frame);
        std::memcpy(&total_frames, data.data() + offset, sizeof(total_frames));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}
