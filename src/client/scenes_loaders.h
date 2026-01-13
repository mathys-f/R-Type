#pragma once

namespace engn {
class EngineContext;
}

void load_game_scene(engn::EngineContext& engine_ctx);
void load_main_menu_scene(engn::EngineContext& engine_ctx);
void load_lobby_scene(engn::EngineContext& engine_ctx);
void load_multiplayer_game_scene(engn::EngineContext& engine_ctx);
void load_settings_menu_scene(engn::EngineContext& engine_ctx);
void load_settings_audio_scene(engn::EngineContext& engine_ctx);
void load_settings_gamepad_menu_scene(engn::EngineContext& engine_ctx);
