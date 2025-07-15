#include <quest/sprite.hpp>

#include <halcyon/utility/guard.hpp>
#include <halcyon/video/renderer.hpp>

using namespace hq;

sprite::sprite(hal::static_texture&& tex, hal::coord::point pos)
    : texture { std::move(tex) }
    , hitbox { pos, *texture.size() } {
}

sprite::sprite(hal::static_texture&& tex, hal::coord::point pos, hal::coord::point size)
    : texture { std::move(tex) }
    , hitbox { pos, size } {
}

void sprite::draw(hal::ref<hal::renderer> rnd) const {
    if (texture.valid()) {
        rnd->draw(texture).to(hitbox).render();
    }
}
