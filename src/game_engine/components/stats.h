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
    Stats(int score, int dmg, int kills, int level = 1, int point_to_next_level = 2500, bool boss_active = false);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
