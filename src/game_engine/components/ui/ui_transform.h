#pragma once

namespace engn {

namespace cpnt {

// 2D Transform component used for UI elements.
// Stores position, z-layer, size, achor point & rotation.
struct UITransform {
    float x;
    float y;
    float z;

    float w;
    float h;

    float anchor_x;
    float anchor_y;

    float rotation;
};

} // namespace cpnt

} // namespace engn
