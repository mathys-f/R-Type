#pragma once

#include "components/i_sync_component.h"
#include "raylib.h"

namespace engn::cpnt {

struct Boss : ISyncComponent {
    // Tag component for enemies

    float timer{};
    float cooldown_1{};
    float cooldown_2{};
    bool time_to_roar{};
    bool roar_active{};
    Vector2 waveCenter{};
    float waveRadius{};
    float waveSpeed{};

    Boss() = default;
    Boss(float timer = 0.0f,
         float cooldown_1 = 0.0f,
         float cooldown_2 = 0.0f,
         bool time_to_roar = false,
         bool roar_active = false,
         Vector2 waveCenter = { 1350.f, 400.f },
         float waveRadius = 0.0f,
         float waveSpeed = 2000.0f);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
