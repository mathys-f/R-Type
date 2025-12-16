#include "systems/client_systems.h"
#include "game_engine/engine.h"

#include "utils/logger.h"

using namespace engn;

static void handle_ui_button_clicked(EngineContext &ctx, const evts::UIButtonClicked &evt);

void handle_main_menu_ui_events(engn::EngineContext& engine_ctx)
{
    const auto &evts = engine_ctx.ui_event_queue;

    evts.for_each<evts::UIButtonClicked>([&engine_ctx](const evts::UIButtonClicked &evt){
        handle_ui_button_clicked(engine_ctx, evt);
    });
}

static void handle_ui_button_clicked(EngineContext &ctx, const evts::UIButtonClicked &evt)
{
    const auto &tags = ctx.registry.tag_registry;
    std::string tag_name = tags.get_tag_name(evt.tag);

    if (tag_name == "play_solo_button") {
        ctx.set_scene(0);
    } else if (tag_name == "play_multiplayer_button") {
        ctx.set_scene(2);
    } else if (tag_name == "setting_button") {
        ctx.set_scene(3);
    } else if (tag_name == "exit_button") {
        ctx.should_quit = true;
    }
}
