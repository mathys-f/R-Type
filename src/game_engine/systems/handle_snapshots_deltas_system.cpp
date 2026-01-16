#include "systems/systems.h"

#include "engine.h"

using namespace engn;

void sys::handle_snapshots_deltas_system(EngineContext& ctx)
{
    ctx.for_each_snapshot_delta([](EngineContext &ctx, const WorldDelta& delta) {
        LOG_DEBUG("Processing snapshot delta based on tick {}", delta.base_snapshot_tick);
    });
}
