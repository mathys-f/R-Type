#pragma once

#include "components/i_sync_component.h"
#include "raylib.h"

namespace engn::cpnt {

struct Boss : ISyncComponent {
    // Tag component for enemies

    float timer{};
    bool roar_active{};
    Vector2 waveCenter{};
    float waveRadius{};
    float waveSpeed{};

    Boss() = default;
    Boss(float time = 0.0f, bool roar = false,
         Vector2 center = { 1350.f, 400.f },
         float radius = 0.0f,
         float speed = 2000.0f);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
