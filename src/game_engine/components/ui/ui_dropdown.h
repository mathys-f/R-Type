#pragma once

#include <vector>
#include <string>

namespace engn {

namespace cpnt {

struct UIDropdown {
    std::vector<std::string> options;
    int selected = -1;
};

} // namespace cpnt

} // namespace engn
