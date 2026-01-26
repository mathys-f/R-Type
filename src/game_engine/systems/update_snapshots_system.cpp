#include "systems/systems.h"

#include "engine.h"

using namespace engn;

void sys::update_snapshots_system(EngineContext& ctx)
{
    // LOG_DEBUG("Updating snapshots acknowledgments");
    std::lock_guard<std::mutex> lock(ctx.snapshots_history_mutex);
    for (auto &[endpoint, history] : ctx.get_snapshots_history()) {
        for (auto &record : history) {
            if (ctx.network_session->is_message_acknowledged(record.msg_id, endpoint) == true)
                record.acknowledged = true;
        }
    }
}
