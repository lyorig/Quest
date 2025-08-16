#include <quest/sprite.hpp>

#include <quest/game.hpp>

#include <halcyon/utility/guard.hpp>
#include <halcyon/video/renderer.hpp>

using namespace hq;

sprite::sprite(hal::coord::point pos)
    : hitbox { pos } {
}

void sprite::atlas_queue(game& g, hal::surface surf) {
    const auto size = surf.size();
    atlas_queue(g, std::move(surf), size);
}

void sprite::atlas_queue(game& g, hal::surface surf, hal::coord::point size) {
    hitbox.size = size;
    g.atlas_queue(std::move(surf), atlas_ref);
}

void sprite::draw(game& g) const {
    // Either dimension being zero-sized means
    // there's no need to render.
    if (hitbox.size.x == 0 || hitbox.size.y == 0) {
        return;
    }

    g.atlas_draw(atlas_ref).to(hitbox).render();
}
