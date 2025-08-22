#include <quest/sprite.hpp>

#include <quest/game.hpp>

#include <halcyon/utility/guard.hpp>
#include <halcyon/video/renderer.hpp>

using namespace hq;

sprite::sprite(game& g, hal::surface surf, hal::coord::point pos)
    : hitbox { pos, surf.size() }
    , m_atlasId { g.atlas_add(std::move(surf)) } {
}

void sprite::draw(game& g) const {
    // Either dimension being zero-sized means
    // there's no need to render.
    if (hitbox.size.x == 0 || hitbox.size.y == 0) {
        return;
    }

    g.atlas_draw(m_atlasId).to(hitbox).render();
}
