#include "systems/systems.h"

#include "utils/logger.h"

#include "engine.h"

using namespace engn;

static void manage_ui_button_clicked(EngineContext &ctx, const evts::UIButtonClicked &evt);

void sys::manage_ui_events(EngineContext &ctx)
{
    const auto &evts = ctx.ui_event_queue;

    evts.for_each<evts::UIButtonClicked>([&ctx](const evts::UIButtonClicked &evt){
        manage_ui_button_clicked(ctx, evt);
    });
}

static void manage_ui_button_clicked(EngineContext &ctx, const evts::UIButtonClicked &evt)
{
    const auto &tags = ctx.registry.tag_registry;

    if (tags.get_tag_name(evt.tag) == "exit_button") {
        ctx.should_quit = true;
    }
}
