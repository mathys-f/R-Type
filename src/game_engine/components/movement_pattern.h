#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct MovementPattern : ISyncComponent {
    enum class PatternType { Straight, Sine, ZigZag, Dive };

    PatternType type{};
    float speed{};
    float amplitude{};
    float frequency{};
    float timer{};
    float base_y{};

    MovementPattern() = default;
    MovementPattern(PatternType type, float speed, float amplitude = 0.0f, 
                   float frequency = 0.0f, float timer = 0.0f, float base_y = 0.0f);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
