#pragma once

// widget.hpp:
// Main menu clickable widgets.

#include <halcyon/video/texture.hpp>

namespace hq {
    class sprite {
    public:
        sprite() = default;

        sprite(hal::static_texture&& tex, hal::coord::point pos);
        sprite(hal::static_texture&& tex, hal::coord::point pos, hal::coord::point size);

        void draw(hal::ref<hal::renderer> rnd) const;

        template <std::invocable<hal::pixel::point> F>
        void reset(hal::static_texture tex, F&& scale_func) {
            texture     = std::move(tex);
            hitbox.size = scale_func(texture.size());
        }

        void pos(hal::coord::point where);

        hal::static_texture texture;
        hal::coord::rect    hitbox;
    };
}
