#include "systems/systems.h"

#include "networking/rtp/networking.h"

#include "engine.h"

using namespace engn;

void sys::send_snapshot_to_client(EngineContext& ctx,
    ecs::SparseArray<cpnt::Replicated> const& replicated_components)
{
    const auto &clients = ctx.get_clients();
}
