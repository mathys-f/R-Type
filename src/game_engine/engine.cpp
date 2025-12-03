#include "engine.h"

#include "components/components.h"

#include "systems/systems.h"

using namespace engn;

EngineContext::EngineContext()
{
    registry.register_component<cpnt::Tag>();

#ifndef HEADLESS
    registry.register_component<cpnt::UIInteractable>();
    registry.register_component<cpnt::UINavigation>();
    registry.register_component<cpnt::UIParent>();
    registry.register_component<cpnt::UIStyle>();
    registry.register_component<cpnt::UITransform>();

    add_system<>(sys::fetch_inputs);
    add_system<>(sys::log_inputs);
#endif
}

void EngineContext::run_systems()
{
    for (auto& sys : m_systems) {
        sys(*this);
    }
}
