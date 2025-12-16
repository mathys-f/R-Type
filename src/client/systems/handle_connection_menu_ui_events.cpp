#include "systems/client_systems.h"
#include "game_engine/engine.h"

#include "utils/logger.h"

using namespace engn;

static void handle_ui_button_clicked(EngineContext &ctx, const evts::UIButtonClicked &e,
    engn::evts::EventQueue<engn::evts::UIEvent> evts_queue);

void handle_connection_menu_ui_events(engn::EngineContext& engine_ctx)
{
    const auto &evts = engine_ctx.ui_event_queue;

    evts.for_each<evts::UIButtonClicked>([&engine_ctx, &evts](const evts::UIButtonClicked &evt){
        handle_ui_button_clicked(engine_ctx, evt, evts);
    });
}

static void handle_ui_button_clicked(EngineContext &ctx, const evts::UIButtonClicked &e,
    engn::evts::EventQueue<engn::evts::UIEvent> evts_queue)
{
    const auto &tags = ctx.registry.tag_registry;
    std::string tag_name = tags.get_tag_name(e.tag);

    if (tag_name == "connect_button") {
        LOG_INFO("Connect button clicked in connection menu");
        // Check the input fields for correct IP & Port
    }
}
