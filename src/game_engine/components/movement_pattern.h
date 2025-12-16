#pragma once

namespace engn {

namespace cpnt {

struct MovementPattern {
    enum class PatternType {
        Straight,
        Sine,
        ZigZag,
        Dive
    };

    PatternType type;
    float speed;
    float amplitude;
    float frequency;
    float timer;
    float baseY;
};

} // namespace cpnt

} // namespace engn
