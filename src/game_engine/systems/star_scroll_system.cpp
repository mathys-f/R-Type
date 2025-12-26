#include "systems/systems.h"

#include "components/components.h"
#include "engine.h"
#include "ecs/zipper.h"
#include "raylib.h"

using namespace engn;

void sys::star_scroll_system(EngineContext &ctx,
                     ecs::SparseArray<cpnt::Transform> const& positions,
                     ecs::SparseArray<cpnt::Star> const& stars)
{
    const float SCROLL_SPEED = ctx.scroll_speed;
    const float WIDTH = ctx.window_size.x;
    const float HEIGHT = ctx.window_size.y;
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, star_opt] : ecs::indexed_zipper(positions, stars)) {
        if (pos_opt && star_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<cpnt::Transform>()[idx];
            
            if (pos) {
                pos->x -= SCROLL_SPEED * (star_opt->z / 1.0f);
                
                if (pos->x <= 0) {
                    pos->x += WIDTH;
                    pos->y = GetRandomValue(0, HEIGHT);
                }
            }
        }
    }
}
