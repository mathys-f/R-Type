#pragma once

#include "ecs/sparse_array.h"
#include "game_engine/components/components.h"

namespace engn {
class EngineContext;
}

void handle_main_menu_ui_events(engn::EngineContext& ctx);
void handle_connection_menu_ui_events(engn::EngineContext& ctx);
void handle_connection_menu_ui_events(engn::EngineContext& engine_ctx);
void handle_settings_menu_ui_events(engn::EngineContext& ctx);
void handle_game_pause_inputs(engn::EngineContext& ctx);
void handle_volume_menu_ui_events(engn::EngineContext& ctx);
