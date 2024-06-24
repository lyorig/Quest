#pragma once

// widget.hpp:
// Main menu clickable widgets.

#include <halcyon/video/texture.hpp>

#include <halcyon/internal/scaler.hpp>

namespace hq {
    class sprite {
    public:
        sprite() = default;

        sprite(hal::static_texture tex, hal::coord::point pos);
        sprite(hal::static_texture tex, hal::coord::point pos, hal::coord::point size);

        void draw(hal::renderer& rnd) const;

        void reset(hal::static_texture tex, hal::scaler scl);

        void pos(hal::coord::point where);

        hal::static_texture texture;
        hal::coord::rect    hitbox;
    };
}