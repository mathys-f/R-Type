#include "components/transform.h"
#include <cstring>

using namespace engn::cpnt;

Transform::Transform(float x, float y, float z,
                    float origin_x, float origin_y,
                    float rx, float ry, float rz,
                    float sx, float sy, float sz)
    : x(x), y(y), z(z), origin_x(origin_x), origin_y(origin_y),
      rx(rx), ry(ry), rz(rz), sx(sx), sy(sy), sz(sz) {}

engn::SerializedComponent Transform::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::transform;
    std::uint32_t size = sizeof(x) + sizeof(y) + sizeof(z) + sizeof(origin_x) + sizeof(origin_y) + 
                      sizeof(rx) + sizeof(ry) + sizeof(rz) + sizeof(sx) + sizeof(sy) + sizeof(sz);
    serialized.data.resize(size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &x, sizeof(x));
    offset += sizeof(x);
    std::memcpy(serialized.data.data() + offset, &y, sizeof(y));
    offset += sizeof(y);
    std::memcpy(serialized.data.data() + offset, &z, sizeof(z));
    offset += sizeof(z);
    std::memcpy(serialized.data.data() + offset, &origin_x, sizeof(origin_x));
    offset += sizeof(origin_x);
    std::memcpy(serialized.data.data() + offset, &origin_y, sizeof(origin_y));
    offset += sizeof(origin_y);
    std::memcpy(serialized.data.data() + offset, &rx, sizeof(rx));
    offset += sizeof(rx);
    std::memcpy(serialized.data.data() + offset, &ry, sizeof(ry));
    offset += sizeof(ry);
    std::memcpy(serialized.data.data() + offset, &rz, sizeof(rz));
    offset += sizeof(rz);
    std::memcpy(serialized.data.data() + offset, &sx, sizeof(sx));
    offset += sizeof(sx);
    std::memcpy(serialized.data.data() + offset, &sy, sizeof(sy));
    offset += sizeof(sy);
    std::memcpy(serialized.data.data() + offset, &sz, sizeof(sz));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Transform::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(x) + sizeof(y) + sizeof(z) + sizeof(origin_x) + sizeof(origin_y) + 
                         sizeof(rx) + sizeof(ry) + sizeof(rz) + sizeof(sx) + sizeof(sy) + sizeof(sz);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&x, data.data() + offset, sizeof(x));
        offset += sizeof(x);
        std::memcpy(&y, data.data() + offset, sizeof(y));
        offset += sizeof(y);
        std::memcpy(&z, data.data() + offset, sizeof(z));
        offset += sizeof(z);
        std::memcpy(&origin_x, data.data() + offset, sizeof(origin_x));
        offset += sizeof(origin_x);
        std::memcpy(&origin_y, data.data() + offset, sizeof(origin_y));
        offset += sizeof(origin_y);
        std::memcpy(&rx, data.data() + offset, sizeof(rx));
        offset += sizeof(rx);
        std::memcpy(&ry, data.data() + offset, sizeof(ry));
        offset += sizeof(ry);
        std::memcpy(&rz, data.data() + offset, sizeof(rz));
        offset += sizeof(rz);
        std::memcpy(&sx, data.data() + offset, sizeof(sx));
        offset += sizeof(sx);
        std::memcpy(&sy, data.data() + offset, sizeof(sy));
        offset += sizeof(sy);
        std::memcpy(&sz, data.data() + offset, sizeof(sz));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}