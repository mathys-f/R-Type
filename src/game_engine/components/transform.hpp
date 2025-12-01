#pragma once

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

    constexpr Transform(float x_ = 0.f, float y_ = 0.f, float z_ = 0.f,
                        float rx_ = 0.f, float ry_ = 0.f, float rz_ = 0.f,
                        float sx_ = 1.f, float sy_ = 1.f, float sz_ = 1.f) noexcept
        : x(x_), y(y_), z(z_),
          rx(rx_), ry(ry_), rz(rz_),
          sx(sx_), sy(sy_), sz(sz_)
    {
    }
};
    