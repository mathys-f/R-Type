#include "components/stats.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Stats::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::stats;
    serialized.size = sizeof(Stats);
    serialized.data.resize(sizeof(Stats));
    std::memcpy(serialized.data.data(), this, sizeof(Stats));
    return serialized;
}

void Stats::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Stats)) {
        std::memcpy(this, data.data(), sizeof(Stats));
    }
}