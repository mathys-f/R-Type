#include "game_engine/engine.h"
#include "systems/client_systems.h"

using namespace engn;

void handle_game_pause_inputs(engn::EngineContext& engine_ctx) {
    const evts::KeyPressed* key_evt = engine_ctx.input_event_queue.get_last<evts::KeyPressed>();
    if (!key_evt)
        return;

    if (key_evt->keycode == evts::KeyboardKeyCode::KeyEscape) {
        engine_ctx.settings_return_scene = engine_ctx.get_current_scene();
        engine_ctx.set_scene(4);
    }
}
