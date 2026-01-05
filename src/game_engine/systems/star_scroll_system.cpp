#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

void sys::star_scroll_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                             ecs::SparseArray<cpnt::Star> const& stars) {
    const float k_scroll_speed = static_cast<float>(ctx.k_scroll_speed);
    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const float k_width = ctx.k_window_size.x;
    const float k_height = ctx.k_window_size.y;
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
    auto& reg = ctx.registry;

    for (auto [idx, pos_opt, star_opt] : ecs::indexed_zipper(positions, stars)) {
        if (pos_opt && star_opt) {
            auto& pos = reg.get_components<cpnt::Transform>()[idx];

            if (pos) {
                pos->x -= k_scroll_speed * (star_opt->z / 1.0f);

                if (pos->x <= 0) {
                    pos->x += k_width;
                    pos->y = static_cast<float>(GetRandomValue(0, static_cast<int>(k_height)));
                }
            }
        }
    }
}
