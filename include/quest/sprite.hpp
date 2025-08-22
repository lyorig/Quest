#pragma once

// sprite.hpp:
// Well, you know what a sprite is, right?

#include <quest/atlas.hpp>

#include <halcyon/video/texture.hpp>

namespace hq {
    class game;

    class sprite {
    public:
        sprite() = default;
        sprite(game& g, hal::surface surf, hal::coord::point pos);

        void draw(game& g) const;

        hal::coord::rect hitbox;

    private:
        texture_atlas::id_t m_atlasId;
    };
}
