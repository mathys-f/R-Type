#include "components/replicated.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Replicated::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::replicated;
    serialized.size = sizeof(Replicated);
    serialized.data.resize(sizeof(Replicated));
    std::memcpy(serialized.data.data(), this, sizeof(Replicated));
    return serialized;
}

void Replicated::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Replicated)) {
        std::memcpy(this, data.data(), sizeof(Replicated));
    }
}