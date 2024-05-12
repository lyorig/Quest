#pragma once

// widget.hpp:
// Main menu clickable widgets.

#include <halcyon/video/texture.hpp>

namespace HQ {
    class sprite {
    public:
        sprite(hal::texture tex, hal::coord_point pos, hal::coord_point size);

        void draw(hal::renderer& rnd) const;

        const hal::coord_rect& hitbox() const;

    private:
        hal::texture    m_tex;
        hal::coord_rect m_hitbox;
    };
}