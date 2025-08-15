#pragma once

#include <halcyon/video/texture.hpp>

#include <rectpack2D/finders_interface.h>

#include <vector>

namespace hq {
    namespace r2d = rectpack2D;

    class texture_atlas {
    public:
        using spaces_t = r2d::empty_spaces<false>;
        using rect_t   = r2d::output_rect_t<spaces_t>;

        texture_atlas() = default;

        // Queue a texture to be added to this atlas by calling `texture_atlas::pack()`.
        void queue(hal::static_texture tex, hal::pixel::rect& out);

        // Free a space from the atlas.
        // This works because atlas rects are unique.
        void free(hal::pixel::rect r);

        // Create the atlas from queued textures.
        void pack(hal::ref<hal::renderer> rnd);

    private:
        struct data {
            hal::static_texture tex;
            hal::pixel::rect*   out;
        };

        // All textures to be added to the atlas.
        std::vector<data>   m_queued;
        std::vector<rect_t> m_taken;

    public:
        // The atlas texture itself. Don't you dare release this.
        hal::target_texture texture;
    };
}
