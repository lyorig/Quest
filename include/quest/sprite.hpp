#pragma once

// widget.hpp:
// Main menu clickable widgets.

#include <halcyon/video/texture.hpp>

#include <halcyon/internal/scaler.hpp>

namespace HQ {
    class sprite {
    public:
        sprite() = default;

        sprite(hal::texture tex, hal::coord::point pos);
        sprite(hal::texture tex, hal::coord::point pos, hal::coord::point size);

        void draw(hal::renderer& rnd) const;

        void reset(hal::texture tex, hal::scaler scl);

        void pos(hal::coord::point where);

        hal::texture     texture;
        hal::coord::rect hitbox;
    };
}