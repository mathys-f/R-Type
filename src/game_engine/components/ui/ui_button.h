#pragma once

#include <functional>

namespace engn {

namespace cpnt {

struct UIButton {
    std::function<void()> onClick;
};

} // namespace cpnt

} // namespace engn
