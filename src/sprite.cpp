#include <quest/sprite.hpp>

#include <halcyon/utility/locks.hpp>
#include <halcyon/video/renderer.hpp>

using namespace HQ;

sprite::sprite(hal::coord::point pos)
    : hitbox { pos } {
}

sprite::sprite(hal::texture tex, hal::coord::point pos, hal::coord::point size)
    : texture { std::move(tex) }
    , hitbox { pos, size } {
}

void sprite::draw(hal::renderer& rnd) const {
    hal::lock::color cl { rnd, hal::palette::green };

    if (texture.valid())
        rnd.render(texture).to(hitbox)();
}

void sprite::reset(hal::texture tex, hal::scaler scl) {
    texture     = std::move(tex);
    hitbox.size = scl(texture.size());
}