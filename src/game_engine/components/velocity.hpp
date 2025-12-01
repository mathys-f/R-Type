#pragma once

/// Velocity component describing linear velocity on each axis.
struct Velocity
{
    float vx;
    float vy;
    float vz;

    /// Construct a Velocity with optional initial components.
    /// @param vx_ Velocity along X.
    /// @param vy_ Velocity along Y.
    /// @param vz_ Velocity along Z.
    constexpr Velocity(float vx_ = 0.f, float vy_ = 0.f, float vz_ = 0.f) noexcept
        : vx(vx_), vy(vy_), vz(vz_)
    {
    }
};