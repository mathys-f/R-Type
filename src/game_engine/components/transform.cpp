#include "components/transform.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Transform::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::transform;
    serialized.size = sizeof(Transform);
    serialized.data.resize(sizeof(Transform));
    std::memcpy(serialized.data.data(), this, sizeof(Transform));
    return serialized;
}

void Transform::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Transform)) {
        std::memcpy(this, data.data(), sizeof(Transform));
    }
}