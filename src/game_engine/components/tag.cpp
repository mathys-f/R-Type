#include "components/tag.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Tag::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::tag;
    serialized.size = sizeof(Tag);
    serialized.data.resize(sizeof(Tag));
    std::memcpy(serialized.data.data(), this, sizeof(Tag));
    return serialized;
}

void Tag::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Tag)) {
        std::memcpy(this, data.data(), sizeof(Tag));
    }
}