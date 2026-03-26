#pragma once

#include "components/i_sync_component.h"
#include "raylib.h"

namespace engn::cpnt {

inline constexpr Vector2 k_default_wave_center{1350.0f, 400.0f};
inline constexpr float k_default_wave_speed = 2000.0f;

struct Boss : ISyncComponent {
    // Tag component for enemies

    float timer{};
    float cooldown_1{};
    float cooldown_2{};
    bool time_to_roar{};
    bool roar_active{};
    Vector2 wave_center{};
    float wave_radius{};
    float wave_speed{};

    Boss() = default;
    Boss(float timer = 0.0f, float cooldown_1 = 0.0f, float cooldown_2 = 0.0f, bool time_to_roar = false,
         bool roar_active = false, Vector2 wave_center = k_default_wave_center, float wave_radius = 0.0f,
         float wave_speed = k_default_wave_speed);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
