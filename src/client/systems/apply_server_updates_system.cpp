#include "client_systems.h"
#include "game_engine/engine.h"
#include "game_engine/systems/systems.h"
#include "utils/logger.h"

void engn::sys::apply_server_updates_system(EngineContext& ctx)
{
    if (!ctx.network_client || !ctx.network_client->is_connected()) {
        LOG_DEBUG("[CLIENT] Skipping server updates: client not connected");
        return;
    }

    handle_snapshots_deltas_system(ctx);
}
