#pragma once

#include "components/i_sync_component.h"

#include <cstddef>

namespace engn::cpnt {

inline constexpr int k_default_point_to_next_level = 2500;

struct Stats : ISyncComponent {
    int score{};
    int dmg{};
    int kills{};
    int level{};
    int point_to_next_level{};
    bool boss_active{};
    int boss_killed{};

    static constexpr std::size_t kSerializedSize = sizeof(int) * 6 + sizeof(bool);

    Stats() = default;
    Stats(int score, int dmg, int kills, int level = 1, int point_to_next_level = 2500, bool boss_active = false,
        int boss_killed = 0);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
