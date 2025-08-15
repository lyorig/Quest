#pragma once

// widget.hpp:
// Main menu clickable widgets.

#include <halcyon/video/texture.hpp>

namespace hq {
    class game;

    class sprite {
    public:
        sprite() = default;

        sprite(game& g, hal::surface surf, hal::coord::point pos);
        sprite(game& g, hal::surface surf, hal::coord::point pos, hal::coord::point size);

        void draw(game& g) const;

        void pos(hal::coord::point where);

        hal::pixel::rect texture;
        hal::coord::rect hitbox;
    };
}
