#pragma once

#include "ecs/sparse_array.h"
#include "game_engine/components/components.h"

#include <cstdint>
#include <memory>
#include <string>

namespace engn {
class EngineContext;
class NetworkClient;
}

void handle_main_menu_ui_events(engn::EngineContext& ctx);
void handle_servers_ui_events(engn::EngineContext& ctx);
void handle_lobby_ui_events(engn::EngineContext& ctx);
void reset_lobby_ui_state();
void reset_servers_ui_state();
void attach_lobby_network_client(std::shared_ptr<engn::NetworkClient> client, const std::string& ip,
                                 std::uint16_t port);
void handle_connection_menu_ui_events(engn::EngineContext& ctx);
void handle_keyboard_settings_menu_ui_events(engn::EngineContext& ctx);
void handle_gamepad_settings_menu_events(engn::EngineContext& ctx);
void handle_game_pause_inputs(engn::EngineContext& ctx);
void handle_volume_menu_ui_events(engn::EngineContext& ctx);
void handle_graphics_settings_menu_ui_events(engn::EngineContext& ctx);
void handle_difficulty_settings_menu_ui_events(engn::EngineContext& ctx);
