#pragma once

#include "ecs/sparse_array.h"
#include "components/components.h"

namespace engn {

class EngineContext;

namespace sys {

void fetch_inputs(EngineContext &ctx);
void log_inputs(EngineContext &ctx);

void ui_transform_resolver(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UIParent> &);

} // namespace systems

} // namespace engine