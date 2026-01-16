#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Stats : ISyncComponent {
    int score{};
    int dmg{};
    int kills{};
    int level{};
    int point_to_next_level{};
    bool boss_active{};

    Stats() = default;
    Stats(int score = 0, int dmg = 0, int kills = 0, int level = 1, int point_to_next_level = 500, bool boss_active = false);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
