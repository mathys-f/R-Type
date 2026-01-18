#include "game_engine/components/tag.h"
#include "game_engine/components/ui/ui_focusable.h"
#include "game_engine/components/ui/ui_navigation.h"
#include "game_engine/engine.h"
#include "game_engine/network_client.h"
#include "networking/lobby/lobby_messages.h"
#include "systems/client_systems.h"
#include "utils/color.h"
#include "utils/logger.h"

#include <memory>
#include <mutex>
#include <optional>
#include <vector>

using namespace engn;

// Store lobby list and network client state
namespace {
constexpr std::size_t k_max_lobby_items_to_display = 5;
constexpr std::size_t k_max_lobby_items_in_list = 10;
constexpr float k_item_height = 50.0f;
constexpr float k_item_spacing = 5.0f;
constexpr float k_start_y = 360.0f;
constexpr float k_start_x = 60.0f;
constexpr float k_lobby_width = 960.0f;
constexpr int k_text_font_size = 24;
constexpr int k_tag_prefix_length = 11; // "lobby_item_"

// UI Colors
constexpr unsigned char k_alpha_opaque = 255U;
constexpr unsigned char k_color_dark_bg = 45U;
constexpr unsigned char k_color_darker_bg = 80U;
constexpr unsigned char k_color_darker_pressed = 35U;
constexpr unsigned char k_color_light_text = 220U;
constexpr unsigned char k_color_white = 255U;
constexpr unsigned char k_color_light_gray = 180U;
constexpr unsigned char k_color_medium_gray = 100U;
constexpr unsigned char k_color_medium_light = 150U;
constexpr unsigned char k_color_medium = 200U;
constexpr float k_border_radius = 0.3f;
constexpr float k_border_thickness = 3.0f;

struct LobbyState {
    std::vector<net::lobby::LobbyInfo> available_lobbies;
    bool connected = false;
    bool waiting_for_response = false;
    std::string status_message = "Not connected";
    std::string server_ip;
    std::uint16_t server_port_main = 0;

    std::optional<net::lobby::ResLobbyList> pending_lobby_list;
    std::optional<net::lobby::ResCreateLobby> pending_create;
    std::optional<net::lobby::ResJoinLobby> pending_join;
    std::optional<std::string> pending_status;
    std::mutex mutex;

    // Cache tag ids for lobby items to avoid recreate errors
    std::vector<ecs::TagRegistry::TagId> lobby_item_tags;
};

LobbyState& get_lobby_state() {
    static LobbyState s_g_lobby_state;
    if (s_g_lobby_state.lobby_item_tags.empty()) {
        s_g_lobby_state.lobby_item_tags.resize(k_max_lobby_items_in_list, ecs::TagRegistry::k_invalid_tag_id);
    }
    return s_g_lobby_state;
}

ecs::TagRegistry::TagId tag_id_or_invalid(const ecs::TagRegistry& tags, const char* name) {
    auto ent_opt = tags.get_entity(name);
    if (!ent_opt.has_value())
        return ecs::TagRegistry::k_invalid_tag_id;
    return tags.get_tag_id(name);
}

void set_navigation_for_tag(EngineContext& ctx, const char* tag, ecs::TagRegistry::TagId up,
                            ecs::TagRegistry::TagId down, ecs::TagRegistry::TagId left,
                            ecs::TagRegistry::TagId right) {
    auto ent_opt = ctx.registry.get_tag_registry().get_entity(tag);
    if (!ent_opt.has_value())
        return;
    ctx.registry.add_component(ent_opt.value(), cpnt::UINavigation{up, down, left, right});
}

void apply_base_lobby_navigation(EngineContext& ctx) {
    const auto& tags = ctx.registry.get_tag_registry();
    auto ip = tag_id_or_invalid(tags, "lobby_server_ip");
    auto port = tag_id_or_invalid(tags, "lobby_server_port");
    auto connect = tag_id_or_invalid(tags, "connect_to_server_button");
    auto refresh = tag_id_or_invalid(tags, "refresh_lobbies_button");
    auto name_input = tag_id_or_invalid(tags, "lobby_name_input");
    auto create = tag_id_or_invalid(tags, "create_lobby_button");
    auto back = tag_id_or_invalid(tags, "back_button");

    set_navigation_for_tag(ctx, "lobby_server_ip", ecs::TagRegistry::k_invalid_tag_id, name_input,
                           ecs::TagRegistry::k_invalid_tag_id, port);
    set_navigation_for_tag(ctx, "lobby_server_port", ecs::TagRegistry::k_invalid_tag_id, name_input, ip, connect);
    set_navigation_for_tag(ctx, "connect_to_server_button", ecs::TagRegistry::k_invalid_tag_id, name_input, port,
                           refresh);
    set_navigation_for_tag(ctx, "refresh_lobbies_button", ecs::TagRegistry::k_invalid_tag_id, create, connect,
                           ecs::TagRegistry::k_invalid_tag_id);
    set_navigation_for_tag(ctx, "lobby_name_input", ip, back, ecs::TagRegistry::k_invalid_tag_id, create);
    set_navigation_for_tag(ctx, "create_lobby_button", refresh, back, name_input, ecs::TagRegistry::k_invalid_tag_id);
    set_navigation_for_tag(ctx, "back_button", create, ip, ecs::TagRegistry::k_invalid_tag_id,
                           ecs::TagRegistry::k_invalid_tag_id);
}

void apply_lobby_list_navigation(EngineContext& ctx, std::size_t lobby_count) {
    if (lobby_count == 0) {
        apply_base_lobby_navigation(ctx);
        return;
    }

    const auto& tags = ctx.registry.get_tag_registry();
    auto name_input = tag_id_or_invalid(tags, "lobby_name_input");
    auto create = tag_id_or_invalid(tags, "create_lobby_button");
    auto back = tag_id_or_invalid(tags, "back_button");

    const auto& tag_cache = get_lobby_state().lobby_item_tags;
    ecs::TagRegistry::TagId first = tag_cache[0];
    ecs::TagRegistry::TagId last = tag_cache[lobby_count - 1];

    set_navigation_for_tag(ctx, "lobby_name_input", tag_id_or_invalid(tags, "lobby_server_ip"), first,
                           ecs::TagRegistry::k_invalid_tag_id, create);
    set_navigation_for_tag(ctx, "create_lobby_button", tag_id_or_invalid(tags, "refresh_lobbies_button"), first,
                           name_input, ecs::TagRegistry::k_invalid_tag_id);
    set_navigation_for_tag(ctx, "back_button", last, tag_id_or_invalid(tags, "lobby_server_ip"),
                           ecs::TagRegistry::k_invalid_tag_id, ecs::TagRegistry::k_invalid_tag_id);

    for (std::size_t i = 0; i < lobby_count; i++) {
        auto tag_id = tag_cache[i];
        if (tag_id == ecs::TagRegistry::k_invalid_tag_id)
            continue;
        auto ent_opt = tags.get_entity(tag_id);
        if (!ent_opt.has_value())
            continue;
        auto up = (i == 0) ? create : tag_cache[i - 1];
        auto down = (i + 1 < lobby_count) ? tag_cache[i + 1] : back;
        ctx.registry.add_component(ent_opt.value(),
                                   cpnt::UINavigation{up, down, ecs::TagRegistry::k_invalid_tag_id,
                                                      ecs::TagRegistry::k_invalid_tag_id});
    }
}

void update_status_text(EngineContext& ctx, const std::string& message) {
    auto status_ent_opt = ctx.registry.get_tag_registry().get_entity("status_text");
    if (status_ent_opt.has_value()) {
        auto& text_components = ctx.registry.get_components<cpnt::UIText>();
        if (auto& text = text_components[status_ent_opt.value()]; text.has_value()) {
            text->content = message;
        }
    }
    get_lobby_state().status_message = message;
}

void clear_lobby_list_ui(EngineContext& ctx) {
    // Remove all existing lobby item entities
    auto& tags = ctx.registry.get_tag_registry();
    auto& state = get_lobby_state();
    for (size_t i = 0; i < state.lobby_item_tags.size(); i++) {
        auto tag_id = state.lobby_item_tags[i];
        if (tag_id == ecs::TagRegistry::k_invalid_tag_id)
            continue;
        auto ent_opt = tags.get_entity(tag_id);
        if (ent_opt.has_value()) {
            ctx.registry.kill_entity(ent_opt.value());
        }
    }
}

void update_lobby_list_ui(EngineContext& ctx) {
    clear_lobby_list_ui(ctx);

    std::vector<net::lobby::LobbyInfo> lobbies;
    {
        auto& s = get_lobby_state();
        std::lock_guard<std::mutex> lock(s.mutex);
        lobbies = s.available_lobbies;
    }

    std::size_t lobby_count = std::min(lobbies.size(), k_max_lobby_items_to_display);
    for (size_t i = 0; i < lobby_count; i++) {
        const auto& lobby = lobbies[i];
        std::string lobby_tag = "lobby_item_" + std::to_string(i);

        // Create or reuse tag binding from cache
        auto& tag_registry = ctx.registry.get_tag_registry();
        auto entity = ctx.registry.spawn_entity();
        auto& tag_cache = get_lobby_state().lobby_item_tags;
        auto tag_id = tag_cache[i];
        if (tag_id != ecs::TagRegistry::k_invalid_tag_id) {
            tag_registry.bind_tag(tag_id, entity);
            ctx.registry.add_component(entity, cpnt::Tag{tag_id});
        } else {
            tag_id = tag_registry.create_and_bind_tag(lobby_tag, entity);
            if (tag_id == ecs::TagRegistry::k_invalid_tag_id) {
                ctx.registry.kill_entity(entity);
                continue;
            }
            tag_cache[i] = tag_id;
            ctx.registry.add_component(entity, cpnt::Tag{tag_id});
        }
        const float k_width = ctx.window_size.x;  // NOLINT(cppcoreguidelines-pro-type-union-access)
        const float k_height = ctx.window_size.y; // NOLINT(cppcoreguidelines-pro-type-union-access)

        // Add UI components
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
        ctx.registry.add_component(
            entity, cpnt::UITransform{
                k_start_x / k_width* 100.0f, 
                (k_start_y + static_cast<float>(i) * (k_item_height + k_item_spacing)) / k_height * 100.0f,
                0.0f, 
                k_lobby_width / k_width* 100.0f, 
                k_item_height / k_height * 100.0f, 
                0.0f, 
                0.0f, 
                0.0f});
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

        std::string lobby_info = lobby.m_lobby_name + " - " + std::to_string(lobby.m_current_players) + "/" +
                                 std::to_string(lobby.m_max_players) + " players";

        ctx.registry.add_component(entity, cpnt::UIText{lobby_info, static_cast<int>(k_text_font_size)});

        ctx.registry.add_component(
            entity,
            cpnt::UIStyle{
                utils::Color{k_color_dark_bg, k_color_dark_bg, k_color_dark_bg, k_alpha_opaque}, // background
                utils::Color{k_color_darker_bg, k_color_darker_bg, k_color_darker_bg,
                             k_alpha_opaque}, // background_hovered
                utils::Color{k_color_darker_pressed, k_color_darker_pressed, k_color_darker_pressed,
                             k_alpha_opaque}, // background_pressed
                utils::Color{k_color_light_text, k_color_light_text, k_color_light_text, k_alpha_opaque}, // text
                utils::Color{k_color_white, k_color_white, k_color_white, k_alpha_opaque}, // text_hovered
                utils::Color{k_color_light_gray, k_color_light_gray, k_color_light_gray,
                             k_alpha_opaque}, // text_pressed
                utils::Color{k_color_medium_gray, k_color_medium_gray, k_color_medium_gray, k_alpha_opaque}, // border
                utils::Color{k_color_medium_light, k_color_medium_light, k_color_medium_light,
                             k_alpha_opaque},                                                 // border_hovered
                utils::Color{k_color_medium, k_color_medium, k_color_medium, k_alpha_opaque}, // border_pressed
                k_border_radius, k_border_thickness});

        ctx.registry.add_component(entity, cpnt::UIButton{});
        ctx.registry.add_component(entity, cpnt::UIInteractable{});
        ctx.registry.add_component(entity, cpnt::UIFocusable{false});
    }

    if (lobbies.empty()) {
        update_status_text(ctx, "No lobbies available. Create one!");
    } else {
        update_status_text(ctx, std::to_string(lobbies.size()) + " lobbies available");
    }

    apply_lobby_list_navigation(ctx, lobby_count);
}

void handle_connect_button(EngineContext& ctx) {
    auto ip_ent_opt = ctx.registry.get_tag_registry().get_entity("lobby_server_ip");
    auto port_ent_opt = ctx.registry.get_tag_registry().get_entity("lobby_server_port");

    if (!ip_ent_opt.has_value() || !port_ent_opt.has_value()) {
        update_status_text(ctx, "Error: Input fields not found");
        return;
    }

    const auto& ip_input = ctx.registry.get_components<cpnt::UIText>()[ip_ent_opt.value()];
    const auto& port_input = ctx.registry.get_components<cpnt::UIText>()[port_ent_opt.value()];

    if (!ip_input.has_value() || !port_input.has_value()) {
        update_status_text(ctx, "Error: Input fields not found");
        return;
    }

    std::string server_ip = ip_input->content;
    std::string port_str = port_input->content;
    uint16_t server_port = static_cast<uint16_t>(std::stoi(port_str));

    // Create network client and connect
    try {
        auto& state = get_lobby_state();
        state.server_ip = server_ip;
        state.server_port_main = server_port;

        ctx.network_client = std::make_shared<engn::NetworkClient>();

        ctx.network_client->set_on_reliable([](const net::Packet& pkt) {
            auto& s = get_lobby_state();
            std::lock_guard<std::mutex> lock(s.mutex);
            if (auto res = net::lobby::parse_res_lobby_list(pkt)) {
                s.pending_lobby_list = *res;
                return;
            }
            if (auto res = net::lobby::parse_res_create_lobby(pkt)) {
                s.pending_create = *res;
                return;
            }
            if (auto res = net::lobby::parse_res_join_lobby(pkt)) {
                s.pending_join = *res;
                return;
            }
        });

        ctx.network_client->set_on_login([&ctx](bool success, uint32_t /*player_id*/) {
            auto& s = get_lobby_state();
            if (success) {
                s.connected = true;
                {
                    std::lock_guard<std::mutex> lock(s.mutex);
                    s.pending_status = "Connected! Requesting lobby list...";
                }

                net::lobby::ReqLobbyList req{};
                ctx.network_client->send_reliable(net::lobby::make_req_lobby_list(req));
            } else {
                std::lock_guard<std::mutex> lock(s.mutex);
                s.pending_status = "Failed to connect to server";
            }
        });

        ctx.network_client->connect(server_ip, server_port, "LobbyBrowser");
        update_status_text(ctx, "Connecting to " + server_ip + ":" + port_str + "...");
    } catch (const std::exception& e) {
        update_status_text(ctx, std::string("Connection error: ") + e.what());
    }
}

void handle_refresh_button(EngineContext& ctx) {
    if (!get_lobby_state().connected || !ctx.network_client) {
        update_status_text(ctx, "Not connected to server");
        return;
    }
    {
        auto& s = get_lobby_state();
        std::lock_guard<std::mutex> lock(s.mutex);
        s.pending_status = "Refreshing lobby list...";
    }
    net::lobby::ReqLobbyList req{};
    ctx.network_client->send_reliable(net::lobby::make_req_lobby_list(req));
    get_lobby_state().waiting_for_response = true;
}

void handle_create_lobby_button(EngineContext& ctx) {
    if (!get_lobby_state().connected || !ctx.network_client) {
        update_status_text(ctx, "Not connected to server");
        return;
    }

    auto name_ent_opt = ctx.registry.get_tag_registry().get_entity("lobby_name_input");
    if (!name_ent_opt.has_value()) {
        update_status_text(ctx, "Error: Lobby name input not found");
        return;
    }

    const auto& name_input = ctx.registry.get_components<cpnt::UIText>()[name_ent_opt.value()];
    if (!name_input.has_value()) {
        update_status_text(ctx, "Error: Lobby name input not found");
        return;
    }

    std::string lobby_name = name_input->content;
    if (lobby_name.empty()) {
        update_status_text(ctx, "Please enter a lobby name");
        return;
    }

    {
        auto& s = get_lobby_state();
        std::lock_guard<std::mutex> lock(s.mutex);
        s.pending_status = "Creating lobby '" + lobby_name + "'...";
    }

    net::lobby::ReqCreateLobby req{};
    req.m_lobby_name = lobby_name;
    req.m_max_players = 4; // Default to 4 players
    ctx.network_client->send_reliable(net::lobby::make_req_create_lobby(req));
    get_lobby_state().waiting_for_response = true;
}

void handle_lobby_item_clicked(EngineContext& ctx, int lobby_index) {
    std::vector<net::lobby::LobbyInfo> lobbies;
    {
        auto& s = get_lobby_state();
        std::lock_guard<std::mutex> lock(s.mutex);
        lobbies = s.available_lobbies;
    }

    if (lobby_index < 0 || lobby_index >= static_cast<int>(lobbies.size())) {
        return;
    }

    if (!get_lobby_state().connected || !ctx.network_client) {
        update_status_text(ctx, "Not connected to server");
        return;
    }

    const auto& lobby = lobbies[static_cast<std::size_t>(lobby_index)];
    {
        auto& s = get_lobby_state();
        std::lock_guard<std::mutex> lock(s.mutex);
        s.pending_status = "Joining lobby '" + lobby.m_lobby_name + "'...";
    }

    net::lobby::ReqJoinLobby req{};
    req.m_lobby_id = lobby.m_lobby_id;
    ctx.network_client->send_reliable(net::lobby::make_req_join_lobby(req));
    get_lobby_state().waiting_for_response = true;
}

} // anonymous namespace

void reset_lobby_ui_state() {
    auto& state = get_lobby_state();
    state.lobby_item_tags.assign(k_max_lobby_items_in_list, ecs::TagRegistry::k_invalid_tag_id);
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& e,
                                     engn::evts::EventQueue<engn::evts::UIEvent> evts_queue);

void handle_lobby_ui_events(engn::EngineContext& engine_ctx) {
    // Poll network client if connected
    if (engine_ctx.network_client && get_lobby_state().connected) {
        engine_ctx.network_client->poll();

        // Process pending lobby messages on the main thread
        std::optional<net::lobby::ResLobbyList> lobby_list;
        std::optional<net::lobby::ResCreateLobby> create_res;
        std::optional<net::lobby::ResJoinLobby> join_res;
        std::optional<std::string> pending_status;

        {
            auto& s = get_lobby_state();
            std::lock_guard<std::mutex> lock(s.mutex);
            lobby_list = std::move(s.pending_lobby_list);
            create_res = std::move(s.pending_create);
            join_res = std::move(s.pending_join);
            pending_status = std::move(s.pending_status);
            s.pending_lobby_list.reset();
            s.pending_create.reset();
            s.pending_join.reset();
            s.pending_status.reset();
        }

        if (pending_status) {
            update_status_text(engine_ctx, *pending_status);
        }

        if (lobby_list) {
            {
                auto& s = get_lobby_state();
                std::lock_guard<std::mutex> lock(s.mutex);
                s.available_lobbies = lobby_list->m_lobbies;
            }
            update_lobby_list_ui(engine_ctx);
            get_lobby_state().waiting_for_response = false;
        }

        if (create_res) {
            if (create_res->m_success) {
                update_status_text(engine_ctx, "Lobby created on port " + std::to_string(create_res->m_port) + " (ID " +
                                                   std::to_string(create_res->m_lobby_id) + ")");
                // Refresh lobby list so it appears immediately
                net::lobby::ReqLobbyList req{};
                engine_ctx.network_client->send_reliable(net::lobby::make_req_lobby_list(req));
            } else {
                update_status_text(engine_ctx, "Failed to create lobby: " + create_res->m_error_message);
            }
            get_lobby_state().waiting_for_response = false;
        }

        if (join_res) {
            if (join_res->m_success) {
                update_status_text(engine_ctx, "Joining lobby...");
                engine_ctx.server_ip = get_lobby_state().server_ip;
                engine_ctx.server_port = join_res->m_port;
                engine_ctx.set_scene("multiplayer_game"); // Multiplayer game scene
            } else {
                update_status_text(engine_ctx, "Failed to join lobby: " + join_res->m_error_message);
            }
            get_lobby_state().waiting_for_response = false;
        }
    }

    const auto& evts = engine_ctx.ui_event_queue;
    evts.for_each<evts::UIButtonClicked>(
        [&engine_ctx, &evts](const evts::UIButtonClicked& evt) { handle_ui_button_clicked(engine_ctx, evt, evts); });
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& e,
                                     engn::evts::EventQueue<engn::evts::UIEvent> /*evts_queue*/) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(e.tag);

    if (tag_name == "connect_to_server_button") {
        handle_connect_button(ctx);
    } else if (tag_name == "refresh_lobbies_button") {
        handle_refresh_button(ctx);
    } else if (tag_name == "create_lobby_button") {
        handle_create_lobby_button(ctx);
    } else if (tag_name == "back_button") {
        ctx.set_scene("main_menu"); // Main menu scene
    } else if (tag_name.find("lobby_item_") == 0) {
        // Extract lobby index from tag name
        int lobby_index = std::stoi(tag_name.substr(k_tag_prefix_length));
        handle_lobby_item_clicked(ctx, lobby_index);
    }
}
