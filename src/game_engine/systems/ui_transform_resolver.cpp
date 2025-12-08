#include "systems.h"

#include "engine.h"

using namespace engn;

void sys::ui_transform_resolver(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UIParent> &parents)
{
    auto &transforms = ctx.registry.get_components<cpnt::UITransform>();
    const auto &tags = ctx.registry.tags();

    for (size_t i = 0; i < transforms.size(); i++) {
        if (!transforms[i]) continue;

        auto &t = transforms[i].value();

        // Initialize parent properties to default values (root-level UI element).
        float parent_x = 0.0f;
        float parent_y = 0.0f;
        float parent_w = ctx.window_size.x; // Default to window width.
        float parent_h = ctx.window_size.y; // Default to window height.

        if (parents[i] && parents[i]->id != ecs::TagRegistry::k_invalid_tag_id) {
            auto parent_entity = ctx.registry.entity_from_index(tags.get_entity(parents[i]->id));

            // If the parent entity has a valid transform, use its properties.
            if (transforms[parent_entity]) {
                const auto &p = transforms[parent_entity].value();
                parent_x = p.x;
                parent_y = p.y;
                parent_w = p.w;
                parent_h = p.h;
            }
        }

        // I first take the parent's position.
        // Then add the anchor offset based on the parent's size.
        // Finally, add the local position.
        t.x = parent_x + parent_w * t.achorX + t.x;
        t.y = parent_y + parent_h * t.achorY + t.y;
    }
}
