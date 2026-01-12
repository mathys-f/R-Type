#include "systems/systems.h"

#include "networking/rtp/networking.h"

#include "engine.h"

using namespace engn;

void sys::send_snapshot_to_client(EngineContext& ctx,
    ecs::SparseArray<cpnt::Replicated> const& replicated_components)
{
    std::vector<asio::ip::udp::endpoint> clients = ctx.m_cli;
}
