#include <quest/sprite.hpp>

#include <quest/game.hpp>

#include <halcyon/utility/guard.hpp>
#include <halcyon/video/renderer.hpp>

using namespace hq;

sprite::sprite(game& g, hal::surface surf, hal::coord::point pos)
    : hitbox { pos, surf.size() } {
    g.atlas_queue(surf, texture);
}

sprite::sprite(game& g, hal::surface surf, hal::coord::point pos, hal::coord::point size)
    : hitbox { pos, size } {
    g.atlas_queue(surf, texture);
}

void sprite::draw(game& g) const {
    if (hitbox.size.x == 0) {
        return;
    }

    g.atlas_draw(texture).to(hitbox).render();
}
