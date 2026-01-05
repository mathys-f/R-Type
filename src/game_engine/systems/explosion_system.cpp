#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

void sys::explosion_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                           ecs::SparseArray<cpnt::Explosion> const& explosions,
                           ecs::SparseArray<cpnt::Sprite> const& sprites) {
    std::vector<ecs::Entity> to_remove;
    auto& reg = ctx.registry;

    for (auto [idx, pos_opt, exp_opt] : ecs::indexed_zipper(positions, explosions)) {
        if (!pos_opt || !exp_opt)
            continue;

        auto& exp = reg.get_components<cpnt::Explosion>()[idx].value();
        auto& spr = reg.get_components<cpnt::Sprite>()[idx].value();

        exp.timer += GetFrameTime();
        if (exp.timer < exp.frame_duration)
            continue;

        exp.timer = 0.0f;
        exp.current_frame++;

        float start_x = spr.source_rect.x;
        float w = spr.source_rect.width;

        float next_x = start_x + w;

        // Stop when next frame would be outside the explosion strip
        if (exp.current_frame >= exp.total_frames) {
            to_remove.push_back(reg.entity_from_index(idx));
        } else {
            spr.source_rect.x = next_x;
        }
    }

    for (auto e : to_remove)
        reg.kill_entity(e);
}
