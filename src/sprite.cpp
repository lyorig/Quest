#include <halcyon/video/renderer.hpp>
#include <quest/sprite.hpp>

using namespace HQ;

sprite::sprite(hal::coord_point pos)
    : hitbox { pos } {
}

sprite::sprite(hal::texture tex, hal::coord_point pos, hal::coord_point size)
    : texture { std::move(tex) }
    , hitbox { pos, size } {
}

void sprite::draw(hal::renderer& rnd) const {
    hal::renderer::color_lock cl { rnd, hal::palette::green };

    if (texture.valid())
        rnd.draw(texture).to(hitbox)();
}

void sprite::reset(hal::texture tex, hal::scaler scl) {
    texture     = std::move(tex);
    hitbox.size = scl(texture.size());
}