#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"

void ExplosionSystem(ecs::Registry& reg,
                     ecs::SparseArray<Position> const& positions,
                     ecs::SparseArray<Explosion> const& explosions,
                     ecs::SparseArray<Sprite> const& sprites)
{
    std::vector<ecs::Entity> to_remove;

    for (auto [idx, pos_opt, exp_opt, sprite_opt] :
         ecs::indexed_zipper(positions, explosions, sprites))
    {
        if (!pos_opt || !exp_opt || !sprite_opt) continue;

        auto& exp = reg.get_components<Explosion>()[idx].value();
        auto& spr = reg.get_components<Sprite>()[idx].value();

        exp.timer += GetFrameTime();
        if (exp.timer < exp.frameDuration) continue;

        exp.timer = 0.0f;
        exp.currentFrame++;

        float startX = spr.sourceRect.x;
        float w = spr.sourceRect.width;

        float nextX = startX + w;

        // Stop when next frame would be outside the explosion strip
        if (exp.currentFrame >= exp.totalFrames)
        {
            to_remove.push_back(reg.entity_from_index(idx));
        }
        else
        {
            spr.sourceRect.x = nextX;
        }
    }

    for (auto e : to_remove)
        reg.kill_entity(e);
}
