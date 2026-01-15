#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
    constexpr int k_level_to_appear = 0;
} // namespace

void sys::boss_system(EngineContext& ctx, ecs::SparseArray<cpnt::Boss> const& boss,
                    ecs::SparseArray<cpnt::Stats> const& stats, ecs::SparseArray<cpnt::BossHitbox> const& boss_hitboxes) {
    std::vector<ecs::Entity> entity_to_kill;
    auto& reg = ctx.registry;
    const int k_width = static_cast<int>(ctx.k_window_size.x);
    const int k_height = static_cast<int>(ctx.k_window_size.y);
    const float maxDist = sqrtf(k_width * k_width + k_height * k_height);

    // Check level
    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (stats_opt) {
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];
            
            if (stat->level >= k_level_to_appear) {
                for (auto [boss_idx, boss_opt] : ecs::indexed_zipper(boss)) {
                    if (boss_opt) {
                        auto& boss_comp = reg.get_components<cpnt::Boss>()[boss_idx];
                        
                        if (!boss_comp->roar_active) {
                            boss_comp->roar_active = true;
                            boss_comp->waveRadius = 0.0f;
                        }
                        
                        // UPDATE wave radius
                        if (boss_comp->roar_active) {
                            boss_comp->waveRadius += boss_comp->waveSpeed * GetFrameTime();
                            
                            if (boss_comp->waveRadius > maxDist) {
                                boss_comp->roar_active = false;
                            }
                        }
                    }
                }
            }
        }
    }
}
