module;

#include <quest/atlas.hpp>
#include <quest/game.hpp>

#include <halcyon/video/texture.hpp>

export module quest.sprite;

export namespace hq {
    // Well, you know what a sprite is, right?
    class sprite {
    public:
        sprite() = default;
        sprite(game& g, hal::surface surf, hal::coord::point pos)
            : hitbox { pos, surf.size() }
            , m_atlasId { g.atlas_add(std::move(surf)) } {
        }

        void draw(game& g) const {
            // Either dimension being zero-sized means
            // there's no need to render.
            if (hitbox.size.x == 0 || hitbox.size.y == 0) {
                return;
            }

            g.atlas_draw(m_atlasId).to(hitbox).render();
        }

        hal::coord::rect hitbox;

    private:
        texture_atlas::id m_atlasId;
    };
}
