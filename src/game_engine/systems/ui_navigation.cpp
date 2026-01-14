#include "engine.h"
#include "events/events.h"
#include "systems/systems.h"

using namespace engn;

namespace {

enum class NavDirection {
    None = 0,
    Up,
    Down,
    Left,
    Right
};

NavDirection read_navigation_direction(const evts::EventQueue<evts::Event>& events) {
    NavDirection direction = NavDirection::None;

    events.for_each<evts::KeyPressed>([&direction](const evts::KeyPressed& evt) {
        if (direction != NavDirection::None)
            return;
        switch (evt.keycode) {
            case evts::KeyboardKeyCode::KeyUp: direction = NavDirection::Up; break;
            case evts::KeyboardKeyCode::KeyDown: direction = NavDirection::Down; break;
            case evts::KeyboardKeyCode::KeyLeft: direction = NavDirection::Left; break;
            case evts::KeyboardKeyCode::KeyRight: direction = NavDirection::Right; break;
            default: break;
        }
    });

    events.for_each<evts::ControllerButtonPressed>([&direction](const evts::ControllerButtonPressed& evt) {
        if (direction != NavDirection::None)
            return;
        switch (evt.button) {
            case evts::ControllerButton::ControllerButtonDpadUp: direction = NavDirection::Up; break;
            case evts::ControllerButton::ControllerButtonDpadDown: direction = NavDirection::Down; break;
            case evts::ControllerButton::ControllerButtonDpadLeft: direction = NavDirection::Left; break;
            case evts::ControllerButton::ControllerButtonDpadRight: direction = NavDirection::Right; break;
            default: break;
        }
    });

    return direction;
}

bool read_confirm_input(const evts::EventQueue<evts::Event>& events) {
    bool confirm = false;
    events.for_each<evts::KeyPressed>([&confirm](const evts::KeyPressed& evt) {
        if (confirm)
            return;
        confirm = evt.keycode == evts::KeyboardKeyCode::KeyEnter || evt.keycode == evts::KeyboardKeyCode::KeySpace;
    });

    events.for_each<evts::ControllerButtonPressed>([&confirm](const evts::ControllerButtonPressed& evt) {
        if (confirm)
            return;
        confirm = evt.button == evts::ControllerButton::ControllerButtonSouth;
    });

    return confirm;
}

bool is_valid_focus_entity(ecs::Entity entity, const ecs::SparseArray<cpnt::UIInteractable>& interactables,
                           const ecs::SparseArray<cpnt::UIFocusable>& focusables) {
    if (entity.value() == 0)
        return false;
    size_t idx = entity.value();
    if (idx >= interactables.size() || idx >= focusables.size())
        return false;
    if (!interactables[idx].has_value() || !focusables[idx].has_value())
        return false;
    return true;
}

std::optional<ecs::Entity> find_first_focusable(const ecs::SparseArray<cpnt::UIInteractable>& interactables,
                                               const ecs::SparseArray<cpnt::UIFocusable>& focusables,
                                               const ecs::Registry& registry) {
    size_t max = std::min(interactables.size(), focusables.size());
    for (size_t i = 1; i < max; i++) {
        if (interactables[i].has_value() && focusables[i].has_value()) {
            return registry.entity_from_index(i);
        }
    }
    return std::nullopt;
}

void apply_focus(ecs::Entity entity, ecs::SparseArray<cpnt::UIInteractable>& interactables,
                 ecs::SparseArray<cpnt::UIFocusable>& focusables, EngineContext& ctx) {
    size_t max = std::min(interactables.size(), focusables.size());
    for (size_t i = 1; i < max; i++) {
        if (interactables[i].has_value())
            interactables[i]->hovered = false;
        if (focusables[i].has_value())
            focusables[i]->is_focused = false;
    }

    size_t idx = entity.value();
    if (idx < interactables.size() && interactables[idx].has_value())
        interactables[idx]->hovered = true;
    if (idx < focusables.size() && focusables[idx].has_value())
        focusables[idx]->is_focused = true;

    ctx.focused_entity = entity;
}

} // namespace

void sys::ui_navigation(EngineContext& ctx, const ecs::SparseArray<cpnt::UIInteractable>& interactables_const,
                        const ecs::SparseArray<cpnt::UIFocusable>& focusables_const,
                        const ecs::SparseArray<cpnt::UINavigation>& navigations) {
    if (ctx.input_context != InputContext::Menu)
        return;
    if (ctx.pending_rebind != ControlAction::None || ctx.pending_gamepad_rebind != GamepadControlAction::None)
        return;

    auto& interactables = ctx.registry.get_components<cpnt::UIInteractable>();
    auto& focusables = ctx.registry.get_components<cpnt::UIFocusable>();
    const auto& inputs = ctx.registry.get_components<cpnt::UIInputField>();
    for (const auto& input : inputs) {
        if (input.has_value() && input->editing)
            return;
    }
    const auto& events = ctx.input_event_queue;
    NavDirection direction = read_navigation_direction(events);
    bool confirm = read_confirm_input(events);

    ecs::Entity focused = ctx.focused_entity;
    if (!is_valid_focus_entity(focused, interactables_const, focusables_const)) {
        auto fallback = find_first_focusable(interactables_const, focusables_const, ctx.registry);
        if (!fallback.has_value())
            return;
        apply_focus(fallback.value(), interactables, focusables, ctx);
        focused = fallback.value();
    }

    if (direction == NavDirection::None && !confirm)
        return;

    if (direction != NavDirection::None) {
        size_t idx = focused.value();
        if (idx >= navigations.size() || !navigations[idx].has_value())
            return;

        const auto& nav = navigations[idx].value();
        ecs::TagRegistry::TagId next_tag = ecs::TagRegistry::k_invalid_tag_id;
        switch (direction) {
            case NavDirection::Up: next_tag = nav.up_tag; break;
            case NavDirection::Down: next_tag = nav.down_tag; break;
            case NavDirection::Left: next_tag = nav.left_tag; break;
            case NavDirection::Right: next_tag = nav.right_tag; break;
            case NavDirection::None: break;
        }

        if (next_tag != ecs::TagRegistry::k_invalid_tag_id) {
            auto next_entity = ctx.registry.get_tag_registry().get_entity(next_tag);
            if (next_entity.has_value() &&
                is_valid_focus_entity(next_entity.value(), interactables_const, focusables_const)) {
                apply_focus(next_entity.value(), interactables, focusables, ctx);
                focused = next_entity.value();
            }
        }
    }

    if (confirm) {
        size_t idx = focused.value();
        const auto& buttons = ctx.registry.get_components<cpnt::UIButton>();
        if (idx < buttons.size() && buttons[idx].has_value()) {
            ctx.ui_event_queue.push(
                evts::UIButtonClicked{ctx.registry.get_tag_registry().get_tag_id(ctx.focused_entity)});
        }
    }
}
