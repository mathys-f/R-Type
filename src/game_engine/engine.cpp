#include "engine.h"

#include <filesystem>

#include "components/components.h"
#include "systems/systems.h"
#include "api/lua.h"
#include "utils/logger.h"

using namespace engn;

const std::string k_scripts_dir = "scripts/";

static void expose_cpp_api(sol::state &lua, EngineContext &ctx);

EngineContext::EngineContext(bool headless)
{
    registry.register_component<cpnt::Bullet>();
    registry.register_component<cpnt::Enemy>();
    registry.register_component<cpnt::Explosion>();
    registry.register_component<cpnt::Health>();
    registry.register_component<cpnt::Hitbox>();
    registry.register_component<cpnt::MovementPattern>();
    registry.register_component<cpnt::Player>();
    registry.register_component<cpnt::Replicated>();
    registry.register_component<cpnt::Sprite>();
    registry.register_component<cpnt::Stats>();
    registry.register_component<cpnt::Tag>();
    registry.register_component<cpnt::Transform>();
    registry.register_component<cpnt::Weapon>();

    add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::bullet_system);
    add_system<cpnt::Transform, cpnt::Bullet, cpnt::Enemy, cpnt::Health, cpnt::Player, cpnt::Hitbox>(sys::collision_system);
    add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::enemy_movement_system);
    add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health, cpnt::Sprite>(sys::enemy_system);
    add_system<cpnt::Transform, cpnt::Explosion, cpnt::Sprite>(sys::explosion_system);
    add_system<cpnt::Transform, cpnt::Velocity, cpnt::Particle, cpnt::Bullet>(sys::particle_emission_system);
    add_system<cpnt::Transform, cpnt::Player, cpnt::Sprite, cpnt::Velocity, cpnt::Health>(sys::player_control_system);
    add_system<cpnt::Transform, cpnt::Sprite, cpnt::Star, cpnt::Velocity, cpnt::Particle>(sys::render_system);
    add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);

    if (!headless) {
        registry.register_component<cpnt::UIButton>();
        registry.register_component<cpnt::UICheckbox>();
        registry.register_component<cpnt::UIDropdown>();
        registry.register_component<cpnt::UIFocusable>();
        registry.register_component<cpnt::UIInputField>();
        registry.register_component<cpnt::UIInteractable>();
        registry.register_component<cpnt::UINavigation>();
        registry.register_component<cpnt::UISlider>();
        registry.register_component<cpnt::UIStyle>();
        registry.register_component<cpnt::UIText>();
        registry.register_component<cpnt::UITransform>();

        registry.register_component<cpnt::Star>();
        registry.register_component<cpnt::Sprite>();
        add_system<>(sys::fetch_inputs);
        // add_system<>(sys::log_inputs);
        add_system<cpnt::UITransform>(sys::ui_hover);
        add_system<>(sys::ui_press);
        add_system<cpnt::UITransform, cpnt::UIStyle>(sys::ui_background_renderer);
        add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle>(sys::ui_text_renderer);
        add_system<>(sys::manage_ui_events);

        lua_ctx = std::make_unique<LuaContext>();
        lua::expose_components(lua_ctx->getLuaState());
        expose_cpp_api(lua_ctx->getLuaState(), *this);
        lua::load_lua_scripts(lua_ctx->getLuaState(), k_scripts_dir);
    }

    registry.spawn_entity(); // ensure entity 0 is reserved
}

void EngineContext::run_systems()
{
    input_event_queue.clear();
    ui_event_queue.clear();
    for (auto& sys : m_systems) {
        sys(*this);
    }
}

static void expose_cpp_api(sol::state &lua, EngineContext &ctx)
{
    LOG_DEBUG("Exposing C++ API to Lua");
    for (const auto &entry : engn::lua::k_api_functions) {
        entry->expose(lua, ctx);
    }
}

void EngineContext::add_scene_loader(std::function<void(EngineContext&)> loader)
{
    m_scenes_loaders.push_back(loader);
}

void EngineContext::set_scene(unsigned char scene_id)
{
    if (scene_id >= m_scenes_loaders.size()) {
        LOG_ERROR("Max number of scenes is 256 (0-255)");
        return;
    } else if (scene_id == m_current_scene) {
        LOG_WARNING("Scene ID {} is already the current scene", static_cast<int>(scene_id));
        return;
    } else if (m_scenes_loaders[scene_id] == nullptr) {
        LOG_ERROR("Scene ID {} has no loader function registered", static_cast<int>(scene_id));
        return;
    }
    LOG_INFO("Switching to scene ID {}", static_cast<int>(scene_id));
    LOG_DEBUG("Clearing registry...");
    registry.~Registry();
    new (&registry) ecs::Registry();
    m_current_scene = scene_id;
    LOG_DEBUG("Loading scene ID {}...", static_cast<int>(scene_id));
    m_scenes_loaders[scene_id](*this);
    LOG_INFO("Scene ID {} loaded", static_cast<int>(scene_id));
}

unsigned char EngineContext::get_current_scene() const
{
    return m_current_scene;
}
