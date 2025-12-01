#pragma once

/// Transform component describing position, rotation and scale of an entity.
/// All members are plain floats representing local transform values.
struct Transform
{
    float x;
    float y;
    float z;

    float rx;
    float ry;
    float rz;

    float sx;
    float sy;
    float sz;

    /// Construct a Transform with optional initial values.
    /// @param x_ X position.
    /// @param y_ Y position.
    /// @param z_ Z position.
    /// @param rx_ Rotation around X axis (degrees or radians per engine convention).
    /// @param ry_ Rotation around Y axis.
    /// @param rz_ Rotation around Z axis.
    /// @param sx_ Scale X.
    /// @param sy_ Scale Y.
    /// @param sz_ Scale Z.
    constexpr Transform(float x_ = 0.f, float y_ = 0.f, float z_ = 0.f,
                        float rx_ = 0.f, float ry_ = 0.f, float rz_ = 0.f,
                        float sx_ = 1.f, float sy_ = 1.f, float sz_ = 1.f) noexcept
        : x(x_), y(y_), z(z_),
          rx(rx_), ry(ry_), rz(rz_),
          sx(sx_), sy(sy_), sz(sz_)
    {
    }
};
    