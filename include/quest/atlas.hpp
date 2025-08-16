#pragma once

#include <halcyon/video/renderer.hpp>

#include <rectpack2D/finders_interface.h>

#include <span>
#include <vector>

namespace hq {
    namespace r2d = rectpack2D;

    class texture_atlas_copyer;

    // A texture atlas.
    // Textures can be either queued to be added, or added immediately.
    // The creation of the atlas itself, called "packing", is done by the
    // eponymous member function `texture_atlas::pack()`. The atlas keeps
    // pointers to "output rects", which it populates upon packing with a
    // given texture's coordinates on the atlas texture. As such, these
    // pointers must be valid for as long as the textures themselves are.
    // If you no longer require a texture, release it with `texture_atlas::free()`,
    // after which its associated output rect is no longer tracked.
    // Several convenience functions also exist, such as `texture_atlas::{add(), replace()}`.
    class texture_atlas {
    public:
        using spaces_t = r2d::empty_spaces<false>;
        using rect_t   = r2d::output_rect_t<spaces_t>;

        texture_atlas() = default;

        // Queue a texture to be added to this atlas manually by `texture_atlas::pack()`.
        void queue(hal::static_texture tex, hal::pixel::rect& out);

        // Immediately create and add a texture to this atlas.
        // Shorthand for `texture_atlas::queue()` and `texture_atlas::pack()`.
        void add(hal::surface surf, hal::pixel::rect& out, hal::ref<hal::renderer> rnd);

        // Immediately replace a texture.
        // Shorthand for `texture_atlas::free()` and `texture_atlas::add()`.
        void replace(hal::surface surf, hal::pixel::rect& out, hal::ref<hal::renderer> rnd);

        // Free a space from the atlas.
        // This works because atlas rects are unique.
        void free(hal::pixel::rect r);

        // Create the atlas from queued textures.
        void pack(hal::ref<hal::renderer> rnd);

        texture_atlas_copyer draw(hal::ref<hal::renderer> rnd, hal::pixel::rect src);

        void debug_draw(
            hal::ref<hal::renderer> rnd,
            hal::coord::point       dst,
            hal::color              outline_atlas,
            hal::color              outline_block) const;

    private:
        struct data {
            rect_t              taken;
            hal::static_texture tex;
            hal::pixel::rect*   out;
        };

        std::vector<data> m_data;

        hal::target_texture create(hal::ref<hal::renderer> rnd, hal::pixel::point sz, std::span<const rect_t> rects);

    public:
        // The atlas texture itself. Don't you dare release this.
        hal::target_texture texture;
    };

    class texture_atlas_copyer : public hal::copyer {
    private:
        using this_ref = texture_atlas_copyer&;

    public:
        // [private] Atlas copyers are created via `texture_atlas::draw()`.
        texture_atlas_copyer(hal::pass_key<texture_atlas>, hal::copyer c);

    public:
        [[nodiscard]] this_ref from(hal::pixel::rect src) {
            src.pos += this->m_posSrc.pos;
            static_cast<void>(copyer::from(src));
            return *this;
        }
    };
}
