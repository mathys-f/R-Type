#include "game_engine/engine.h"
#include "systems/client_systems.h"

using namespace engn;

void handle_game_pause_inputs(engn::EngineContext& engine_ctx) {
    const evts::KeyPressed* key_evt = engine_ctx.input_event_queue.get_last<evts::KeyPressed>();
    const evts::ControllerButtonPressed* pad_evt =
        engine_ctx.input_event_queue.get_last<evts::ControllerButtonPressed>();
    const bool k_escape_pressed = key_evt && key_evt->keycode == evts::KeyboardKeyCode::KeyEscape;
    const bool k_pause_pressed =
        pad_evt && (pad_evt->button == evts::ControllerButton::ControllerButtonStart ||
                    pad_evt->button == evts::ControllerButton::ControllerButtonBack);

    if (k_escape_pressed || k_pause_pressed) {
        engine_ctx.settings_return_scene = engine_ctx.get_current_scene();
        engine_ctx.set_scene(4);
    }
}
