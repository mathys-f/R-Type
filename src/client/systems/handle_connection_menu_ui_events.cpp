#include "game_engine/engine.h"
#include "systems/client_systems.h"
#include "utils/logger.h"

using namespace engn;

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& e,
                                     engn::evts::EventQueue<engn::evts::UIEvent> evts_queue);

void handle_connection_menu_ui_events(engn::EngineContext& engine_ctx) {
    const auto& evts = engine_ctx.ui_event_queue;

    evts.for_each<evts::UIButtonClicked>(
        [&engine_ctx, &evts](const evts::UIButtonClicked& evt) { handle_ui_button_clicked(engine_ctx, evt, evts); });
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& e,
                                     engn::evts::EventQueue<engn::evts::UIEvent> evts_queue) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(e.tag);

    if (tag_name == "connect_button") {
        auto ip_ent_opt = ctx.registry.get_tag_registry().get_entity("ip_input_field");
        auto port_ent_opt = ctx.registry.get_tag_registry().get_entity("port_input_field");
        if (!ip_ent_opt.has_value() || !port_ent_opt.has_value()) {
            LOG_ERROR("Input field entities not found for connection menu");
            return;
        }

        const auto& ip_input_fields = ctx.registry.get_components<cpnt::UIText>()[ip_ent_opt.value()];
        const auto& port_input_fields = ctx.registry.get_components<cpnt::UIText>()[port_ent_opt.value()];
        if (!ip_input_fields.has_value() || !port_input_fields.has_value()) {
            LOG_ERROR("Input fields not found for connection menu");
            return;
        }
        std::string server_ip = ip_input_fields->content;
        std::string port_str = port_input_fields->content;
        uint16_t server_port = static_cast<uint16_t>(std::stoi(port_str));
        ctx.server_ip = server_ip;
        ctx.server_port = server_port;
        ctx.set_scene(3); // Multiplayer game scene
        return;
    }
    if (tag_name == "back_button") {
        ctx.set_scene(1); // Main menu scene
        return;
    }
}
