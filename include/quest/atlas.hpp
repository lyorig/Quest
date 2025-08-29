#pragma once

#include <halcyon/video/renderer.hpp>

#include <rectpack2D/finders_interface.h>

#include <vector>

namespace hq {
    namespace r2d = rectpack2D;

    class texture_atlas_copyer;

    // A texture atlas.
    //
    // The creation of the atlas itself, called "packing", is done by the
    // eponymous member function `texture_atlas::pack()`. The atlas keeps
    // a sparse `std::vector` of rects, which it populates upon packing with a
    // given texture's coordinates on the atlas texture. You receive an ID, which
    // is implemented as nothing more than an index into the rect vector.
    //
    // If you no longer require a texture, release it with `texture_atlas::free()`.
    // Convenience functions also exist, such as `texture_atlas::replace()`.
    class texture_atlas {
    public:
        using spaces_t = r2d::empty_spaces<false>;
        using rect_t   = r2d::output_rect_t<spaces_t>;

        using id_t = std::uint8_t;

        texture_atlas();

        // Create and queue a texture for this atlas.
        // Returns the texture ID to draw with.
        id_t add(hal::ref<hal::renderer> rnd, hal::surface surf);

        // Replace a texture. Use if you're unsure whether their dimensions are gonna be different.
        // Shorthand for `texture_atlas::free()` and `texture_atlas::add()`.
        void replace(id_t id, hal::ref<hal::renderer> rnd, hal::surface surf);

        // Replace a texture with an exact-size one.
        // This is particularly efficient as you don't need to repack.
        void replace_exact(id_t id, hal::ref<hal::renderer> rnd, hal::surface surf);

        // Free a space from the atlas.
        // This works because atlas rects are unique.
        void free(id_t id);

        // Create the atlas from queued textures.
        void pack(hal::ref<hal::renderer> rnd);

        // Returns a specialized `hal::copyer`.
        texture_atlas_copyer draw(id_t id, hal::ref<hal::renderer> rnd);

        // Get the area of a texture in the atlas.
        hal::pixel::rect area(id_t id) const;

        void debug_draw(
            hal::ref<hal::renderer> rnd,
            hal::coord::point       dst,
            hal::color              outline_atlas,
            hal::color              outline_block) const;

    private:
        struct data {
            hal::pixel::rect    area;   // Where the texture lies.
            rect_t              staged; // Temporary storage for repacking.
            hal::static_texture tex;    // The texture source (only valid when queued)

            rect_t&       get_rect();
            const rect_t& get_rect() const;

            void invalidate();
            bool valid() const;
        };

        std::vector<data> m_data;

    public:
        hal::target_texture texture;

    private:
        bool m_repack;

        hal::target_texture create(hal::ref<hal::renderer> rnd, hal::pixel::point sz);
    };

    class texture_atlas_copyer : public hal::copyer {
    public:
        // [private] Atlas copyers are created via `texture_atlas::draw()`.
        texture_atlas_copyer(hal::pass_key<texture_atlas>, hal::copyer c);

    public:
        // An overload that applies the correct offset to the source rect..
        [[nodiscard]] texture_atlas_copyer& from(hal::pixel::rect src);
    };
}
