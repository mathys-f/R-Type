#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Stats : ISyncComponent {
    int score{};
    int dmg{};
    int kills{};
    int level{};

    Stats() = default;
    Stats(int score = 0, int dmg = 0, int kills = 0, int level = 0);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
