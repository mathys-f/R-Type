#pragma once

#include "../assets_manager.h"
#include "raylib.h"

namespace engn::cpnt {

struct Sprite {
    Rectangle source_rect;
    float scale;
    int frame;
    AssetsManager::Asset texture;
};

} // namespace engn::cpnt
