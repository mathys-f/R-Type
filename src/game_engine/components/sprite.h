#pragma once

#include "raylib.h"

#include "../assets_manager.h"
#include "components/snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Sprite {
    Rectangle source_rect;
    float scale;
    int frame;
    AssetsManager::Asset texture;

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
