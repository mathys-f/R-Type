#pragma once

namespace engn {

namespace cpnt {

struct Explosion {
    enum class ExplosionType {
        Small,
        Large
    };

    ExplosionType type;
    float timer;
    float frameDuration;
    int currentFrame;
    int totalFrames;
};

} // namespace cpnt

} // namespace engn
