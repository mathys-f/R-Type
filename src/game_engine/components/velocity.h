#pragma once

struct Velocity
{
    float vx;
    float vy;
    float vz;

    constexpr Velocity(float vx_ = 0.f, float vy_ = 0.f, float vz_ = 0.f) noexcept
        : vx(vx_), vy(vy_), vz(vz_)
    {
    }
};