#pragma once

#include <halcyon/video/texture.hpp>

#include <rectpack2D/finders_interface.h>

#include <vector>

namespace hq {
    namespace r2d = rectpack2D;

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

        // Queue a texture to be added to this atlas by calling `texture_atlas::pack()`.
        void queue(hal::static_texture tex, hal::pixel::rect& out);

        // Immediately add a texture to this atlas.
        // Shorthand for `texture_atlas::queue()` and `texture_atlas::pack()`.
        void add(hal::static_texture tex, hal::pixel::rect& out, hal::ref<hal::renderer> rnd);

        // Immediately replace a texture.
        // Shorthand for `texture_atlas::free()` and `texture_atlas::add()`.
        void replace(hal::static_texture tex, hal::pixel::rect& out, hal::ref<hal::renderer> rnd);

        // Free a space from the atlas.
        // This works because atlas rects are unique.
        void free(hal::pixel::rect r);

        // Create the atlas from queued textures.
        void pack(hal::ref<hal::renderer> rnd);

    private:
        // It's unfortunate that there have to be
        // three separate arrays, but rectpack2D
        // only takes a `std::vector<rect_t>` as a parameter.

        std::vector<hal::static_texture> m_queued;
        std::vector<rect_t>              m_taken;
        std::vector<hal::pixel::rect*>   m_output;

    public:
        // The atlas texture itself. Don't you dare release this.
        hal::target_texture texture;
    };
}
