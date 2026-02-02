#include "systems/client_systems.h"
#include "game_engine/engine.h"
#include "game_engine/components/components.h"
#include "utils/logger.h"

using namespace engn;

void handle_disconnect_ui_events(engn::EngineContext& ctx) {
    const auto& evts = ctx.ui_event_queue;
    const auto& tags = ctx.registry.get_tag_registry();

    evts.for_each<evts::UIButtonClicked>([&ctx, &tags](const evts::UIButtonClicked& evt) {
        std::string tag_name = tags.get_tag_name(evt.tag);
        if (tag_name == "disconnect_back_button") {
            ctx.set_scene("main_menu");
        }
    });
}
