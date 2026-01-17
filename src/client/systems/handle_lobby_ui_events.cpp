#include "game_engine/components/tag.h"
#include "game_engine/components/ui/ui_clip.h"
#include "game_engine/components/ui/ui_focusable.h"
#include "game_engine/components/ui/ui_navigation.h"
#include "game_engine/components/ui/ui_scroll_area.h"
#include "game_engine/components/ui/ui_transform.h"
#include "game_engine/engine.h"
#include "game_engine/network_client.h"
#include "networking/lobby/lobby_messages.h"
#include "systems/client_systems.h"
#include "utils/color.h"
#include "utils/logger.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

using namespace engn;

// Store lobby list and network client state
namespace {
constexpr float k_item_height_px = 50.0f;
constexpr float k_item_spacing_px = 8.0f;
constexpr float k_list_padding_px = 10.0f;
constexpr float k_scroll_step_px = k_item_height_px + k_item_spacing_px;
constexpr float k_scroll_epsilon_px = 0.01f;
constexpr float k_scrollbar_width_px = 8.0f;
constexpr float k_scrollbar_padding_px = 2.0f;
constexpr float k_lobby_right_margin_px = 16.0f;
constexpr float k_scrollbar_min_thumb_px = 14.0f;
constexpr float k_scrollbar_radius = 0.2f;
constexpr float k_scrollbar_border_thickness = 0.0f;
constexpr unsigned char k_scrollbar_track_color = 25U;
constexpr unsigned char k_scrollbar_thumb_color = 140U;
constexpr unsigned char k_scrollbar_track_alpha = 140U;
constexpr unsigned char k_scrollbar_thumb_alpha = 220U;
constexpr unsigned char k_scrollbar_hidden_alpha = 0U;
constexpr float k_uninitialized_scroll_offset_px = -1.0f;
constexpr std::size_t k_uninitialized_create_tick = std::numeric_limits<std::size_t>::max();
constexpr std::size_t k_digit_base_10 = 10;
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

struct UiRectPx {
    float x;
    float y;
    float w;
    float h;
};

struct LobbyState {
    std::shared_ptr<NetworkClient> network_client;
    std::vector<net::lobby::LobbyInfo> available_lobbies;
    bool connected = false;
    bool waiting_for_response = false;
    bool auto_connect_attempted = false;
    bool lobby_list_requested = false;
    std::size_t last_create_tick = k_uninitialized_create_tick;
    bool preserve_connection = false;
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
    float last_scroll_offset_px = k_uninitialized_scroll_offset_px;
};

LobbyState& get_lobby_state() {
    static LobbyState s_g_lobby_state;
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
    auto refresh = tag_id_or_invalid(tags, "refresh_lobbies_button");
    auto name_input = tag_id_or_invalid(tags, "lobby_name_input");
    auto create = tag_id_or_invalid(tags, "create_lobby_button");
    auto back = tag_id_or_invalid(tags, "back_button");

    set_navigation_for_tag(ctx, "refresh_lobbies_button", ecs::TagRegistry::k_invalid_tag_id, name_input,
                           ecs::TagRegistry::k_invalid_tag_id, ecs::TagRegistry::k_invalid_tag_id);
    set_navigation_for_tag(ctx, "lobby_name_input", refresh, back, ecs::TagRegistry::k_invalid_tag_id, create);
    set_navigation_for_tag(ctx, "create_lobby_button", refresh, back, name_input, ecs::TagRegistry::k_invalid_tag_id);
    set_navigation_for_tag(ctx, "back_button", create, refresh, ecs::TagRegistry::k_invalid_tag_id,
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

    set_navigation_for_tag(ctx, "lobby_name_input", tag_id_or_invalid(tags, "refresh_lobbies_button"), first,
                           ecs::TagRegistry::k_invalid_tag_id, create);
    set_navigation_for_tag(ctx, "create_lobby_button", tag_id_or_invalid(tags, "refresh_lobbies_button"), first,
                           name_input, ecs::TagRegistry::k_invalid_tag_id);
    set_navigation_for_tag(ctx, "back_button", last, tag_id_or_invalid(tags, "refresh_lobbies_button"),
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

void connect_to_server(EngineContext& ctx, const std::string& server_ip, std::uint16_t server_port) {
    if (server_ip.empty() || server_port == 0) {
        update_status_text(ctx, "Invalid server address");
        return;
    }

    try {
        auto& state = get_lobby_state();
        state.server_ip = server_ip;
        state.server_port_main = server_port;
        state.lobby_list_requested = false;

        state.network_client = std::make_shared<NetworkClient>();

        state.network_client->set_on_reliable([](const net::Packet& pkt) {
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

        state.network_client->set_on_login([&ctx](bool success, uint32_t /*player_id*/) {
            auto& s = get_lobby_state();
            if (success) {
                if (s.connected && s.lobby_list_requested)
                    return;
                s.connected = true;
                {
                    std::lock_guard<std::mutex> lock(s.mutex);
                    s.pending_status = "Connected! Requesting lobby list...";
                }

                if (!s.lobby_list_requested) {
                    s.lobby_list_requested = true;
                    s.waiting_for_response = true;
                    net::lobby::ReqLobbyList req{};
                    s.network_client->send_reliable(net::lobby::make_req_lobby_list(req));
                }
            } else {
                std::lock_guard<std::mutex> lock(s.mutex);
                s.pending_status = "Failed to connect to server";
            }
        });

        state.network_client->connect(server_ip, server_port, "LobbyBrowser");
        update_status_text(ctx, "Connecting to " + server_ip + ":" + std::to_string(server_port) + "...");
    } catch (const std::exception& e) {
        update_status_text(ctx, std::string("Connection error: ") + e.what());
    }
}
const cpnt::UITransform* get_lobby_list_transform(EngineContext& ctx) {
    auto list_ent_opt = ctx.registry.get_tag_registry().get_entity("lobby_list_bg");
    if (!list_ent_opt.has_value())
        return nullptr;
    auto& transforms = ctx.registry.get_components<cpnt::UITransform>();
    const auto& list_transform = transforms[list_ent_opt.value()];
    if (!list_transform.has_value())
        return nullptr;
    return &list_transform.value();
}

std::optional<UiRectPx> get_lobby_list_rect_px(EngineContext& ctx, const cpnt::UITransform& list_transform) {
    const float k_width = ctx.window_size.x;  // NOLINT(cppcoreguidelines-pro-type-union-access)
    const float k_height = ctx.window_size.y; // NOLINT(cppcoreguidelines-pro-type-union-access)

    UiRectPx rect{
        list_transform.x / 100.0f * k_width,
        list_transform.y / 100.0f * k_height,
        list_transform.w / 100.0f * k_width,
        list_transform.h / 100.0f * k_height
    };
    return rect;
}

std::optional<ecs::Entity> get_lobby_list_entity(EngineContext& ctx) {
    return ctx.registry.get_tag_registry().get_entity("lobby_list_bg");
}

cpnt::UIScrollArea& ensure_lobby_scroll_area(EngineContext& ctx, ecs::Entity list_entity) {
    auto& scroll_areas = ctx.registry.register_component<cpnt::UIScrollArea>();
    const std::size_t k_idx = list_entity.value();
    if (k_idx >= scroll_areas.size() || !scroll_areas[k_idx].has_value()) {
        ctx.registry.add_component(list_entity, cpnt::UIScrollArea{});
    }
    auto& scroll_area_opt = scroll_areas[k_idx];

    scroll_area_opt->padding_px = k_list_padding_px;
    scroll_area_opt->scroll_step_px = k_item_height_px + k_item_spacing_px;
    scroll_area_opt->scrollbar_width_px = k_scrollbar_width_px;
    scroll_area_opt->scrollbar_padding_px = k_scrollbar_padding_px;
    scroll_area_opt->scrollbar_min_thumb_px = k_scrollbar_min_thumb_px;
    scroll_area_opt->scrollbar_radius = k_scrollbar_radius;
    scroll_area_opt->scrollbar_border_thickness = k_scrollbar_border_thickness;
    scroll_area_opt->scrollbar_hidden_alpha = k_scrollbar_hidden_alpha;
    scroll_area_opt->scrollbar_track_color = utils::Color{k_scrollbar_track_color, k_scrollbar_track_color,
                                                          k_scrollbar_track_color, k_scrollbar_track_alpha};
    scroll_area_opt->scrollbar_thumb_color = utils::Color{k_scrollbar_thumb_color, k_scrollbar_thumb_color,
                                                          k_scrollbar_thumb_color, k_scrollbar_thumb_alpha};
    return scroll_area_opt.value();
}

float px_to_percent_x(const EngineContext& ctx, float px) {
    return (px / ctx.window_size.x) * 100.0f; // NOLINT(cppcoreguidelines-pro-type-union-access)
}

float px_to_percent_y(const EngineContext& ctx, float px) {
    return (px / ctx.window_size.y) * 100.0f; // NOLINT(cppcoreguidelines-pro-type-union-access)
}

float lobby_content_height_px(std::size_t lobby_count) {
    if (lobby_count == 0)
        return 0.0f;
    return static_cast<float>(lobby_count) * (k_item_height_px + k_item_spacing_px) - k_item_spacing_px;
}

float clamp_scroll_offset(float offset_px, float max_scroll_px) {
    if (max_scroll_px <= 0.0f)
        return 0.0f;
    return std::clamp(offset_px, 0.0f, max_scroll_px);
}

bool parse_lobby_index(const std::string& tag_name, std::size_t& out_index) {
    if (tag_name.rfind("lobby_item_", 0) != 0)
        return false;
    const std::string k_index_str = tag_name.substr(k_tag_prefix_length);
    if (k_index_str.empty())
        return false;
    std::size_t value = 0;
    for (char c : k_index_str) {
        if (c < '0' || c > '9')
            return false;
        value = (value * k_digit_base_10) + static_cast<std::size_t>(c - '0');
    }
    out_index = value;
    return true;
}

bool has_navigation_input(const evts::EventQueue<evts::Event>& events) {
    bool has_nav = false;
    events.for_each<evts::KeyPressed>([&has_nav](const evts::KeyPressed& evt) {
        if (has_nav)
            return;
        has_nav = (evt.keycode == evts::KeyboardKeyCode::KeyUp || evt.keycode == evts::KeyboardKeyCode::KeyDown);
    });
    events.for_each<evts::ControllerButtonPressed>([&has_nav](const evts::ControllerButtonPressed& evt) {
        if (has_nav)
            return;
        has_nav = (evt.button == evts::ControllerButton::ControllerButtonDpadUp ||
                   evt.button == evts::ControllerButton::ControllerButtonDpadDown);
    });
    return has_nav;
}

void apply_focus_to_entity(EngineContext& ctx, ecs::Entity entity) {
    if (entity.value() == 0)
        return;
    auto& interactables = ctx.registry.get_components<cpnt::UIInteractable>();
    auto& focusables = ctx.registry.get_components<cpnt::UIFocusable>();
    const std::size_t k_max = std::min(interactables.size(), focusables.size());
    for (std::size_t i = 1; i < k_max; ++i) {
        if (interactables[i].has_value())
            interactables[i]->hovered = false;
        if (focusables[i].has_value())
            focusables[i]->is_focused = false;
    }
    const std::size_t k_idx = entity.value();
    if (k_idx < interactables.size() && interactables[k_idx].has_value())
        interactables[k_idx]->hovered = true;
    if (k_idx < focusables.size() && focusables[k_idx].has_value())
        focusables[k_idx]->is_focused = true;
    ctx.focused_entity = entity;
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

    const auto k_list_ent_opt = get_lobby_list_entity(ctx);
    if (!k_list_ent_opt.has_value())
        return;
    const auto* list_transform = get_lobby_list_transform(ctx);
    if (!list_transform)
        return;
    auto list_rect_opt = get_lobby_list_rect_px(ctx, *list_transform);
    if (!list_rect_opt)
        return;

    const UiRectPx k_list_rect = *list_rect_opt;
    const float k_clip_x_px = k_list_rect.x;
    const float k_clip_y_px = k_list_rect.y;
    const float k_clip_w_px = k_list_rect.w;
    const float k_clip_h_px = k_list_rect.h;

    auto& scroll_area = ensure_lobby_scroll_area(ctx, k_list_ent_opt.value());
    scroll_area.content_height_px = lobby_content_height_px(lobbies.size());

    const float k_viewport_height_px = std::max(0.0f, k_clip_h_px - (scroll_area.padding_px * 2.0f));
    scroll_area.scroll_offset_px =
        clamp_scroll_offset(scroll_area.scroll_offset_px, std::max(0.0f, scroll_area.content_height_px - k_viewport_height_px));

    std::size_t lobby_count = lobbies.size();
    auto& state = get_lobby_state();
    auto& tag_cache = state.lobby_item_tags;
    if (tag_cache.size() < lobby_count) {
        tag_cache.resize(lobby_count, ecs::TagRegistry::k_invalid_tag_id);
    }

    const float k_item_x_px = k_clip_x_px + scroll_area.padding_px;
    const float k_item_w_px = std::max(0.0f, k_clip_w_px - scroll_area.padding_px * 2.0f - k_scrollbar_width_px -
                                                 k_lobby_right_margin_px);
    const float k_start_y_px = k_clip_y_px + scroll_area.padding_px - scroll_area.scroll_offset_px;

    for (size_t i = 0; i < lobby_count; i++) {
        const auto& lobby = lobbies[i];
        std::string lobby_tag = "lobby_item_" + std::to_string(i);

        // Create or reuse tag binding from cache
        auto& tag_registry = ctx.registry.get_tag_registry();
        auto entity = ctx.registry.spawn_entity();
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
        // Add UI components
        const float k_item_y_px = k_start_y_px + static_cast<float>(i) * (k_item_height_px + k_item_spacing_px);
        ctx.registry.add_component(
            entity, cpnt::UITransform{px_to_percent_x(ctx, k_item_x_px), px_to_percent_y(ctx, k_item_y_px), 0.0f,
                                      px_to_percent_x(ctx, k_item_w_px), px_to_percent_y(ctx, k_item_height_px), 0.0f,
                                      0.0f, 0.0f});

        std::string lobby_info = lobby.m_lobby_name + " - " + std::to_string(lobby.m_current_players) + "/" +
                                 std::to_string(lobby.m_max_players) + " players";

        ctx.registry.add_component(entity, cpnt::UIText{lobby_info, static_cast<int>(k_text_font_size)});

        const float k_clip_x = px_to_percent_x(ctx, k_clip_x_px);
        const float k_clip_y = px_to_percent_y(ctx, k_clip_y_px);
        const float k_clip_w = px_to_percent_x(ctx, k_clip_w_px);
        const float k_clip_h = px_to_percent_y(ctx, k_clip_h_px);
        ctx.registry.add_component(entity, cpnt::UIClip{k_clip_x, k_clip_y, k_clip_w, k_clip_h});
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

    state.last_scroll_offset_px = scroll_area.scroll_offset_px;
    apply_lobby_list_navigation(ctx, lobby_count);
}

void handle_refresh_button(EngineContext& ctx) {
    if (!get_lobby_state().connected || !get_lobby_state().network_client) {
        update_status_text(ctx, "Not connected to server");
        return;
    }
    {
        auto& s = get_lobby_state();
        std::lock_guard<std::mutex> lock(s.mutex);
        s.pending_status = "Refreshing lobby list...";
    }
    net::lobby::ReqLobbyList req{};
    get_lobby_state().network_client->send_reliable(net::lobby::make_req_lobby_list(req));
    get_lobby_state().waiting_for_response = true;
}

void handle_create_lobby_button(EngineContext& ctx) {
    auto& state = get_lobby_state();
    const std::size_t k_current_tick = ctx.get_current_tick();
    if (state.last_create_tick == k_current_tick) {
        return;
    }
    if (state.waiting_for_response) {
        update_status_text(ctx, "Please wait for the server response");
        return;
    }
    if (!get_lobby_state().connected || !get_lobby_state().network_client) {
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
        std::lock_guard<std::mutex> lock(state.mutex);
        state.pending_status = "Creating lobby '" + lobby_name + "'...";
    }

    net::lobby::ReqCreateLobby req{};
    req.m_lobby_name = lobby_name;
    req.m_max_players = 4; // Default to 4 players
    state.last_create_tick = k_current_tick;
    state.waiting_for_response = true;
    state.network_client->send_reliable(net::lobby::make_req_create_lobby(req));
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

    if (!get_lobby_state().connected || !get_lobby_state().network_client) {
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
    get_lobby_state().network_client->send_reliable(net::lobby::make_req_join_lobby(req));
    get_lobby_state().waiting_for_response = true;
}

void ensure_focused_lobby_visible(EngineContext& ctx) {
    auto& state = get_lobby_state();
    if (!has_navigation_input(ctx.input_event_queue))
        return;

    const auto k_focused = ctx.focused_entity;
    if (k_focused.value() == 0)
        return;

    const auto& tag_components = ctx.registry.get_components<cpnt::Tag>();
    const std::size_t k_focused_idx = k_focused.value();
    if (k_focused_idx >= tag_components.size() || !tag_components[k_focused_idx].has_value())
        return;

    const auto k_tag_id = tag_components[k_focused_idx]->id;
    const std::string k_tag_name = ctx.registry.get_tag_registry().get_tag_name(k_tag_id);
    std::size_t k_lobby_index = 0;
    if (!parse_lobby_index(k_tag_name, k_lobby_index))
        return;

    std::size_t k_lobby_count = 0;
    {
        std::lock_guard<std::mutex> lock(state.mutex);
        k_lobby_count = state.available_lobbies.size();
    }
    if (k_lobby_index >= k_lobby_count)
        return;

    const auto k_list_ent_opt = get_lobby_list_entity(ctx);
    if (!k_list_ent_opt.has_value())
        return;
    const auto* k_list_transform = get_lobby_list_transform(ctx);
    if (!k_list_transform)
        return;
    const auto k_list_rect_opt = get_lobby_list_rect_px(ctx, *k_list_transform);
    if (!k_list_rect_opt)
        return;

    const UiRectPx k_list_rect = *k_list_rect_opt;
    auto& scroll_area = ensure_lobby_scroll_area(ctx, k_list_ent_opt.value());
    const float k_viewport_height_px = std::max(0.0f, k_list_rect.h - (scroll_area.padding_px * 2.0f));
    const float k_content_height_px = lobby_content_height_px(k_lobby_count);
    const float k_max_scroll_px = std::max(0.0f, k_content_height_px - k_viewport_height_px);

    const float k_item_top_px = static_cast<float>(k_lobby_index) * (k_item_height_px + k_item_spacing_px);
    const float k_item_bottom_px = k_item_top_px + k_item_height_px;

    float k_next_offset_px = scroll_area.scroll_offset_px;
    if (k_item_top_px < k_next_offset_px) {
        k_next_offset_px = k_item_top_px;
    } else if (k_item_bottom_px > k_next_offset_px + k_viewport_height_px) {
        k_next_offset_px = k_item_bottom_px - k_viewport_height_px;
    }
    k_next_offset_px = clamp_scroll_offset(k_next_offset_px, k_max_scroll_px);

    if (std::abs(k_next_offset_px - scroll_area.scroll_offset_px) < k_scroll_epsilon_px)
        return;

    scroll_area.scroll_offset_px = k_next_offset_px;
    update_lobby_list_ui(ctx);

    const auto k_refocus_entity = ctx.registry.get_tag_registry().get_entity(k_tag_id);
    if (k_refocus_entity.has_value()) {
        apply_focus_to_entity(ctx, k_refocus_entity.value());
    }
}

void update_lobby_list_on_scroll(EngineContext& ctx) {
    auto& state = get_lobby_state();
    const auto k_list_ent_opt = get_lobby_list_entity(ctx);
    if (!k_list_ent_opt.has_value())
        return;
    auto& scroll_areas = ctx.registry.get_components<cpnt::UIScrollArea>();
    const std::size_t k_list_idx = k_list_ent_opt->value();
    if (k_list_idx >= scroll_areas.size() || !scroll_areas[k_list_idx].has_value())
        return;
    const float k_current_offset = scroll_areas[k_list_idx]->scroll_offset_px;
    if (std::abs(k_current_offset - state.last_scroll_offset_px) < k_scroll_epsilon_px)
        return;
    update_lobby_list_ui(ctx);
}

} // anonymous namespace

void attach_lobby_network_client(std::shared_ptr<NetworkClient> client, const std::string& ip, std::uint16_t port) {
    auto& state = get_lobby_state();
    state.network_client = std::move(client);
    state.server_ip = ip;
    state.server_port_main = port;
    state.connected = true;
    state.waiting_for_response = false;
    state.lobby_list_requested = true;
    state.auto_connect_attempted = true;
    state.preserve_connection = true;

    if (!state.network_client) {
        return;
    }

    state.network_client->set_on_reliable([](const net::Packet& pkt) {
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

    {
        std::lock_guard<std::mutex> lock(state.mutex);
        state.pending_status = "Connected! Requesting lobby list...";
    }

    net::lobby::ReqLobbyList req{};
    state.network_client->send_reliable(net::lobby::make_req_lobby_list(req));
    state.waiting_for_response = true;
}

void reset_lobby_ui_state() {
    auto& state = get_lobby_state();
    if (state.preserve_connection) {
        state.preserve_connection = false;
        state.available_lobbies.clear();
    } else {
        state.network_client.reset();
        state.available_lobbies.clear();
        state.connected = false;
        state.waiting_for_response = false;
        state.pending_lobby_list.reset();
        state.pending_create.reset();
        state.pending_join.reset();
        state.pending_status.reset();
        state.lobby_list_requested = false;
    }
    state.lobby_item_tags.clear();
    state.last_scroll_offset_px = k_uninitialized_scroll_offset_px;
    state.auto_connect_attempted = false;
    state.last_create_tick = k_uninitialized_create_tick;
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& e,
                                     engn::evts::EventQueue<engn::evts::UIEvent> evts_queue);

void handle_lobby_ui_events(engn::EngineContext& engine_ctx) {
    if (engine_ctx.get_current_scene() != "lobby") {
        return;
    }

    auto& state = get_lobby_state();
    if (!state.auto_connect_attempted && !state.connected && !state.network_client &&
        !engine_ctx.server_ip.empty() && engine_ctx.server_port != 0) {
        state.auto_connect_attempted = true;
        connect_to_server(engine_ctx, engine_ctx.server_ip, engine_ctx.server_port);
    }

    // Poll network client if connected
    if (state.network_client && state.connected) {
        state.network_client->poll();

        // Process pending lobby messages on the main thread
        std::optional<net::lobby::ResLobbyList> lobby_list;
        std::optional<net::lobby::ResCreateLobby> create_res;
        std::optional<net::lobby::ResJoinLobby> join_res;
        std::optional<std::string> pending_status;

        {
            std::lock_guard<std::mutex> lock(state.mutex);
            lobby_list = std::move(state.pending_lobby_list);
            create_res = std::move(state.pending_create);
            join_res = std::move(state.pending_join);
            pending_status = std::move(state.pending_status);
            state.pending_lobby_list.reset();
            state.pending_create.reset();
            state.pending_join.reset();
            state.pending_status.reset();
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
                state.network_client->send_reliable(net::lobby::make_req_lobby_list(req));
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
                return;
            } else {
                update_status_text(engine_ctx, "Failed to join lobby: " + join_res->m_error_message);
            }
            get_lobby_state().waiting_for_response = false;
        }
    }

    if (engine_ctx.get_current_scene() != "lobby") {
        return;
    }
    update_lobby_list_on_scroll(engine_ctx);
    ensure_focused_lobby_visible(engine_ctx);

    const auto& evts = engine_ctx.ui_event_queue;
    evts.for_each<evts::UIButtonClicked>(
        [&engine_ctx, &evts](const evts::UIButtonClicked& evt) { handle_ui_button_clicked(engine_ctx, evt, evts); });
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& e,
                                     engn::evts::EventQueue<engn::evts::UIEvent> /*evts_queue*/) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(e.tag);

    if (tag_name == "refresh_lobbies_button") {
        handle_refresh_button(ctx);
    } else if (tag_name == "create_lobby_button") {
        handle_create_lobby_button(ctx);
    } else if (tag_name == "back_button") {
        auto& state = get_lobby_state();
        if (state.network_client) {
            state.network_client->disconnect();
        }
        state.preserve_connection = false;
        reset_lobby_ui_state();
        ctx.set_scene("servers"); // Servers scene
    } else if (tag_name.find("lobby_item_") == 0) {
        // Extract lobby index from tag name
        int lobby_index = std::stoi(tag_name.substr(k_tag_prefix_length));
        handle_lobby_item_clicked(ctx, lobby_index);
    }
}
