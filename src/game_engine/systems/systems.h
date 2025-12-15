#pragma once

#include "ecs/sparse_array.h"
#include "components/components.h"

namespace engn {

class EngineContext;

namespace sys {

void fetch_inputs(EngineContext &ctx);
void log_inputs(EngineContext &ctx);

void ui_hover(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UITransform> &);
void ui_press(EngineContext &ctx);
void ui_background_renderer(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UITransform> &,
    const ecs::SparseArray<cpnt::UIStyle> &
);
void ui_text_renderer(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UITransform> &,
    const ecs::SparseArray<cpnt::UIText> &,
    const ecs::SparseArray<cpnt::UIStyle> &
);

} // namespace systems

} // namespace engine
