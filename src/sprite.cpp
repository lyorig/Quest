#include <halcyon/video/renderer.hpp>
#include <quest/sprite.hpp>

using namespace HQ;

Sprite::Sprite(hal::texture tex, hal::coord_point pos, hal::coord_point size)
    : m_tex { std::move(tex) }
    , m_hitbox { pos, size } {
}

void Sprite::Draw(hal::renderer& rnd) const {
    hal::renderer::color_lock cl { rnd, hal::palette::green };

    rnd.draw(m_tex).to(m_hitbox)();
}

const hal::coord_rect& Sprite::Hitbox() const {
    return m_hitbox;
}
