#pragma once

#include <halcyon/video/renderer.hpp>

#include <rectpack2D/finders_interface.h>

#include <future>
#include <vector>

namespace hq {
    namespace r2d = rectpack2D;

    class game;
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

        // Make it less obvious that this is nothing more than an index
        // into an array.
        enum class id : std::uint8_t;

        texture_atlas();

        // Create and queue a texture for this atlas.
        // Returns the texture ID to draw with.
        id add(game& g, hal::surface surf);

        // Replace a texture. Use if you're unsure whether their dimensions are gonna be different.
        void replace(id id, game& g, hal::surface surf);

        // Replace a texture with an exact-size one.
        // This is particularly efficient as you can just draw directly onto the texture.
        void replace_exact(id id, hal::ref<hal::renderer> rnd, hal::surface surf);

        // Free a space from the atlas.
        // This works because atlas rects are unique.
        void free(id id);

        // Create the atlas from queued textures.
        void pack(hal::ref<hal::renderer> rnd);

        // Perform garbage collection, which is probably a misnomer, but at least it sounds cool.
        // Removes all unused fields off the end of the data vector.
        void gc();

        // Returns a specialized `hal::copyer`.
        texture_atlas_copyer draw(id id, hal::ref<hal::renderer> rnd);

        // Get the area of a texture in the atlas.
        hal::pixel::rect area(id id) const;

        void debug_draw(
            hal::ref<hal::renderer> rnd,
            hal::coord::point       dst,
            hal::color              outline_atlas,
            hal::color              outline_block) const;

    private:
        struct data {
            hal::pixel::rect area;   // Where the texture lies.
            rect_t           staged; // Temporary storage for repacking.

            // The texture source.
            // Textures are created asynchronously.
            std::future<hal::static_texture> tex;

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
