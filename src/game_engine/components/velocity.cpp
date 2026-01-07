#include "components/velocity.h"
#include <cstring>

using namespace engn::cpnt;

Velocity::Velocity(float vx, float vy, float vz, float vrx, float vry, float vrz)
    : vx(vx), vy(vy), vz(vz), vrx(vrx), vry(vry), vrz(vrz) {}

engn::SerializedComponent Velocity::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::velocity;
    serialized.size = sizeof(this->vx) + sizeof(this->vy) + sizeof(this->vz) + sizeof(this->vrx) + sizeof(this->vry) + sizeof(this->vrz);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &this->vx, sizeof(this->vx));
    offset += sizeof(this->vx);
    std::memcpy(serialized.data.data() + offset, &this->vy, sizeof(this->vy));
    offset += sizeof(this->vy);
    std::memcpy(serialized.data.data() + offset, &this->vz, sizeof(this->vz));
    offset += sizeof(this->vz);
    std::memcpy(serialized.data.data() + offset, &this->vrx, sizeof(this->vrx));
    offset += sizeof(this->vrx);
    std::memcpy(serialized.data.data() + offset, &this->vry, sizeof(this->vry));
    offset += sizeof(this->vry);
    std::memcpy(serialized.data.data() + offset, &this->vrz, sizeof(this->vrz));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Velocity::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(this->vx) + sizeof(this->vy) + sizeof(this->vz) + sizeof(this->vrx) + sizeof(this->vry) + sizeof(this->vrz);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&this->vx, data.data() + offset, sizeof(this->vx));
        offset += sizeof(this->vx);
        std::memcpy(&this->vy, data.data() + offset, sizeof(this->vy));
        offset += sizeof(this->vy);
        std::memcpy(&this->vz, data.data() + offset, sizeof(this->vz));
        offset += sizeof(this->vz);
        std::memcpy(&this->vrx, data.data() + offset, sizeof(this->vrx));
        offset += sizeof(this->vrx);
        std::memcpy(&this->vry, data.data() + offset, sizeof(this->vry));
        offset += sizeof(this->vry);
        std::memcpy(&this->vrz, data.data() + offset, sizeof(this->vrz));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}