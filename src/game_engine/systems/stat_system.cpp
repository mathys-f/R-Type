#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

using namespace engn;

void sys::stat_system(EngineContext& ctx, ecs::SparseArray<cpnt::Stats> const& stats) {
    auto& reg = ctx.registry;

    // Debug - show stats
    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (stats_opt) {
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];

            if (stat->score >= stat->point_to_next_level) {
                stat->level += 1;
                stat->point_to_next_level += 1000; // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
            }
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
            // NOLINTBEGIN(cppcoreguidelines-pro-type-vararg,-warnings-as-errors)
            DrawText(TextFormat("Score: %d", stat->score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Enemies killed: %d", stat->kills), 10, 40, 20, WHITE);
            DrawText(TextFormat("Level: %d", stat->level), 10, 70, 20, WHITE);
            DrawText(TextFormat("Points to next level: %d", stat->point_to_next_level), 10, 100, 20, WHITE);
            // NOLINTEND(cppcoreguidelines-pro-type-vararg,-warnings-as-errors)
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
        }
    }
}
