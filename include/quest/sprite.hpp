#pragma once

// sprite.hpp:
// Well, you know what a sprite is, right?

#include <halcyon/video/texture.hpp>

namespace hq {
    class game;

    class sprite {
    public:
        sprite() = default;

        sprite(hal::coord::point pos);

        void atlas_queue(game& g, hal::surface surf);
        void atlas_queue(game& g, hal::surface surf, hal::coord::point size);

        void draw(game& g) const;

        hal::pixel::rect atlas_ref;
        hal::coord::rect hitbox;
    };
}
