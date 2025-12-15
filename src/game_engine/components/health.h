#pragma once

#include <array>
#include <cstddef>

namespace engn {

namespace cpnt {

struct Health {
    int hp;
    int maxHp;
    
    /// Here, changeCount is the number of pending health changes
    /// kMaxChangeQueue is the maximum size of the change queue
    static constexpr std::size_t kMaxChangeQueue = 32u;
    std::array<int, kMaxChangeQueue> changeQueue{};
    std::size_t changeCount = 0u;
};

} // namespace component

} // namespace engine
