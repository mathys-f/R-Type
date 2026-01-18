#pragma once

#include "components/i_sync_component.h"
#include <string>

namespace engn::cpnt {

struct EntityType : ISyncComponent {
    std::string type_name;

    EntityType() = default;
    explicit EntityType(const std::string& type_name);
    explicit EntityType(std::string&& type_name);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
