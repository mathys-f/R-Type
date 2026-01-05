#pragma once

#include <string>
#include <vector>

namespace engn {

namespace cpnt {

struct UIDropdown {
    std::vector<std::string> options;
    int selected = -1;
    bool expanded = false;
};

} // namespace cpnt

} // namespace engn
