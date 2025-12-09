#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"

#define WIDTH               1920
#define HEIGHT              1000
#define SCROLL_SPEED        12

void StarScrollSystem(ecs::Registry& reg, 
                     ecs::SparseArray<Position> const& positions,
                     ecs::SparseArray<Star> const& stars) {
    for (auto [idx, pos_opt, star_opt] : ecs::indexed_zipper(positions, stars)) {
        if (pos_opt && star_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<Position>()[idx];
            
            if (pos) {
                pos->x -= SCROLL_SPEED * (star_opt->z / 1);
                
                if (pos->x <= 0) {
                    pos->x += WIDTH;
                    pos->y = GetRandomValue(0, HEIGHT);
                }
            }
        }
    }
}