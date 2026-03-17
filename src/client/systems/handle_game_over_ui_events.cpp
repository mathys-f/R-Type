#include "game_engine/engine.h"
#include "systems/client_systems.h"

using namespace engn;

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt);

void handle_game_over_ui_events(engn::EngineContext& engine_ctx) {
    const auto& evts = engine_ctx.ui_event_queue;

    evts.for_each<evts::UIButtonClicked>(
        [&engine_ctx](const evts::UIButtonClicked& evt) { handle_ui_button_clicked(engine_ctx, evt); });
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(evt.tag);

    if (tag_name == "retry_button") {
        if (ctx.game_over_retry_scene.empty()) {
            ctx.set_scene("main_menu");
            return;
        }

        ctx.set_scene(ctx.game_over_retry_scene);
    } else if (tag_name == "main_menu_button") {
        ctx.set_scene("main_menu");
    } else if (tag_name == "exit_button") {
        ctx.should_quit = true;
    }
}