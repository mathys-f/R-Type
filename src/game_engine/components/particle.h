#pragma once

namespace engn::cpnt {

struct Particle {
    float lifetime{};
    float max_lifetime{};
    unsigned char red{};
    unsigned char green{};
    unsigned char blue{};
};

} // namespace engn::cpnt
