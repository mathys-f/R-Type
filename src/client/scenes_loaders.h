#pragma once

namespace engn {
class EngineContext;
}

void load_game_scene(engn::EngineContext& engine_ctx);
void load_main_menu_scene(engn::EngineContext& engine_ctx);
void load_servers_scene(engn::EngineContext& engine_ctx);
void load_lobby_scene(engn::EngineContext& engine_ctx);
void load_multiplayer_game_scene(engn::EngineContext& engine_ctx);
void load_keyboard_settings_menu_scene(engn::EngineContext& engine_ctx);
void load_audio_settings_scene(engn::EngineContext& engine_ctx);
void load_gamepad_settings_menu_scene(engn::EngineContext& engine_ctx);
void load_graphics_settings_scene(engn::EngineContext& engine_ctx);
void load_difficulty_settings_scene(engn::EngineContext& engine_ctx);
