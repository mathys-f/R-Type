#pragma once

namespace engn {
class EngineContext;
}

void load_main_menu_scene(engn::EngineContext& engine_ctx);
void load_game_scene(engn::EngineContext& engine_ctx);
void load_multiplayer_game_scene(engn::EngineContext& engine_ctx);
void load_lobby_scene(engn::EngineContext& engine_ctx);
