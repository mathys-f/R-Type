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

} // namespace systems

} // namespace engine