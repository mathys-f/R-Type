#include "components/entity_type.h"
#include <cstring>

using namespace engn::cpnt;

EntityType::EntityType(const std::string& type_name) : type_name(type_name) {}

EntityType::EntityType(std::string&& type_name) : type_name(std::move(type_name)) {}

engn::SerializedComponent EntityType::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::entity_type;

    // Serialize string length + string data
    std::uint32_t str_length = static_cast<std::uint32_t>(type_name.size());
    std::uint32_t size = sizeof(str_length) + str_length;
    serialized.data.resize(size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &str_length, sizeof(str_length));
    offset += sizeof(str_length);
    std::memcpy(serialized.data.data() + offset, type_name.data(), str_length);
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    return serialized;
}

void EntityType::deserialize(const std::vector<std::byte>& data) {
    if (data.size() < sizeof(std::uint32_t)) {
        return;
    }

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::uint32_t str_length = 0;
    std::memcpy(&str_length, data.data() + offset, sizeof(str_length));
    offset += sizeof(str_length);

    if (data.size() >= offset + str_length) {
        type_name.resize(str_length);
        std::memcpy(type_name.data(), data.data() + offset, str_length);
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}
