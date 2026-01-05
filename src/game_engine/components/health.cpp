#include "components/health.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Health::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::health;
    serialized.size = sizeof(Health);
    serialized.data.resize(sizeof(Health));
    std::memcpy(serialized.data.data(), this, sizeof(Health));
    return serialized;
}

void Health::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Health)) {
        std::memcpy(this, data.data(), sizeof(Health));
    }
}