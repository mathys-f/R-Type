#pragma once

#include "raylib.h"

#include "assets_manager.h"

namespace engn {

namespace cpnt {

struct Sprite {
    Rectangle source_rect;
    float scale;
    int frame;
    AssetsManager::Asset texture;
};

} // namespace cpnt

} // namespace engn
