#include "engine.h"

#include "components/components.h"

#include "systems/systems.h"

using namespace engn;

EngineContext::EngineContext(bool headless)
{
    registry.register_component<cpnt::Tag>();

    if (!headless) {
        lua_ctx = std::make_unique<LuaContext>();
        registry.register_component<cpnt::UIInteractable>();
        registry.register_component<cpnt::UINavigation>();
        registry.register_component<cpnt::UIStyle>();
        registry.register_component<cpnt::UITransform>();

        add_system<>(sys::fetch_inputs);
        // add_system<>(sys::log_inputs);
        add_system<cpnt::UITransform>(sys::ui_hover);
        add_system<>(sys::ui_press);
    }
}

void EngineContext::run_systems()
{
    for (auto& sys : m_systems) {
        sys(*this);
    }
}
