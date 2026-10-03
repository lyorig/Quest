module;

#include <halcyon/surface.hpp>
#include <halcyon/system.hpp>
#include <halcyon/utility/guard.hpp>
#include <halcyon/video/renderer.hpp>
#include <halcyon/video/texture.hpp>

#include <bit>
#include <cstdint>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

#include <rectpack2D/finders_interface.h>

export module quest.atlas;

export namespace hq {
    namespace r2d = rectpack2D;

    namespace atlas_consts {
        constexpr std::size_t  max_side { 1024 };
        constexpr std::int32_t discard_step { -4 };
    }

    class texture_atlas;

    class texture_atlas_copyer : public hal::copyer {
    public:
        // [private] Atlas copyers are created via `texture_atlas::draw()`.
        texture_atlas_copyer(hal::pass_key<texture_atlas>, hal::copyer c)
            : hal::copyer { std::move(c) } {
        }

        // An overload that applies the correct offset to the source rect..
        [[nodiscard]] texture_atlas_copyer& from(hal::pixel::rect src) {
            src.pos += this->m_posSrc.pos;
            return static_cast<texture_atlas_copyer&>(copyer::from(src));
        }
    };

    // A texture atlas.
    //
    // The creation of the atlas itself, called "packing", is done by the
    // eponymous member function `texture_atlas::pack()`. The atlas keeps
    // a sparse `std::vector` of rects, which it populates upon packing with
    // a given texture's coordinates on the atlas texture. You receive an ID,
    // which is implemented as nothing more than an index into the rect vector.
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

        texture_atlas()
            : m_shouldRepack { false } { }

        // Create and queue a texture for this atlas.
        // Returns the texture ID to draw with.
        id add(hal::ref<hal::renderer> rnd, hal::surface surf) {
            m_shouldRepack = true;

            const rect_t        rect { to_r2d(surf.size()) };
            hal::static_texture tex { rnd, std::move(surf) };

            std::underlying_type_t<id> i { 0 };

            // Can we reuse a slot in the vector?
            for (; i < m_data.size(); ++i) {
                data& d { m_data[i] };

                if (!d.valid()) {
                    d.tex    = hal::static_texture { rnd, std::move(surf) };
                    d.staged = rect;

                    return static_cast<id>(i);
                }
            }

            m_data.emplace_back(
                hal::pixel::rect {},
                rect,
                hal::static_texture { rnd, std::move(surf) });

            return static_cast<id>(i);
        }

        // Replace a texture. Use if you're unsure whether their dimensions are gonna be different.
        void replace(id id, hal::ref<hal::renderer> rnd, hal::surface surf) {
            data& d { m_data[std::to_underlying(id)] };

            if (surf.size() == d.area) {
                return replace_exact(id, rnd, std::move(surf));
            }

            m_shouldRepack = true;

            d.staged = to_r2d(surf.size());
            d.tex    = hal::static_texture { rnd, std::move(surf) };
        }

        // Replace a texture with an exact-size one.
        // This is particularly efficient as you can just draw directly onto the texture.
        void replace_exact(id id, hal::ref<hal::renderer> rnd, hal::surface surf) {
            hal::guard::target  _ { rnd, texture };
            hal::static_texture tex { rnd, std::move(surf) };

            rnd->draw(tex).to(m_data[std::to_underlying(id)].area).render();
        }

        // Free a space from the atlas.
        // This works because atlas rects are unique.
        void free(id id) {
            m_data[std::to_underlying(id)].invalidate();
        }

        // Create the atlas from queued textures.
        void pack(hal::ref<hal::renderer> rnd) {
            using cr = r2d::callback_result;

            // Check whether there's actually anything to do.
            if (!m_shouldRepack) {
                return;
            }

            m_shouldRepack = false;

            // Find the best possible packing for these rects...
            const r2d::rect_wh size { r2d::find_best_packing<spaces_t>(
                m_data,
                r2d::make_finder_input(
                    atlas_consts::max_side,
                    atlas_consts::discard_step,
                    [](const rect_t&) { return cr::CONTINUE_PACKING; },
                    [](const rect_t&) { return cr::ABORT_PACKING; },
                    r2d::flipping_option::DISABLED)) };

            // ...then create the texture itself.
            texture = create(rnd, std::bit_cast<hal::pixel::point>(size));
        }

        // Perform garbage collection, which is probably a misnomer, but at least it sounds cool.
        // Removes all unused fields off the end of the data vector.
        void gc() {
            const auto subrange {
                std::ranges::find_last_if_not(m_data, &data::valid)
            };

            m_data.erase(subrange.begin(), subrange.end());
        }

        // Returns a specialized `hal::copyer`.
        texture_atlas_copyer draw(id id, hal::ref<hal::renderer> rnd) {
            HAL_ASSERT(m_data[std::to_underlying(id)].valid(), "Drawing invalid texture");

            return { hal::pass_key<texture_atlas> {}, rnd->draw(texture).from(m_data[std::to_underlying(id)].area) };
        }

        // Get the area of a texture in the atlas.
        hal::pixel::rect area(id id) const {
            return m_data[std::to_underlying(id)].area;
        }

        void debug_draw(
            hal::ref<hal::renderer> rnd,
            hal::coord::point       dst,
            hal::color              outline_atlas,
            hal::color              outline_block) const {
            rnd->draw(texture).to(dst).render();

            for (const data& d : m_data) {
                if (!d.valid()) {
                    continue;
                }

                hal::pixel::rect rect { d.area };
                rect.pos += dst;
                rect.pos += { 1, 1 };
                rect.size -= { 2, 2 };

                rnd->draw(rect, outline_block);
            }

            rnd->draw({ dst, texture.size().get() }, outline_atlas);
        }

    private:
        struct data {
            hal::pixel::rect area;   // Where the texture lies.
            rect_t           staged; // Temporary storage for repacking.

            // The texture source.
            hal::static_texture tex;

            rect_t& get_rect() {
                return staged;
            }

            const rect_t& get_rect() const {
                return staged;
            }

            void invalidate() {
                staged.x = -1;
            }

            bool valid() const {
                return staged.x != -1;
            }
        };

        static constexpr hal::pixel::rect to_hal(texture_atlas::rect_t r) {
            return std::bit_cast<hal::pixel::rect>(r);
        }

        static constexpr texture_atlas::rect_t to_r2d(hal::pixel::point size) {
            return { 0, 0, size.x, size.y };
        }

        std::vector<data> m_data;

    public:
        hal::target_texture texture;

    private:
        // If `false`, `pack()` is a no-op.
        // Used to optimize game loop iterations in which
        // no textures are queued to be added.
        bool m_shouldRepack;

        hal::target_texture create(hal::ref<hal::renderer> rnd, hal::pixel::point sz) {
            hal::target_texture canvas { rnd, sz };
            canvas.blend(hal::blend_mode::additive_premul);

            // FIXME: Metal apparently waits for VSync upon setting the render target.
            // https://discourse.libsdl.org/t/SDL-setrendertargets-metal-implementation-apparently-waits-for-vsync/62009.
            hal::guard::target _ { rnd, canvas };

            // Clear the texture (added after noticing weird graphical glitches on Windows).
            rnd->clear();

            for (data& d : m_data) {
                // Skip unused entries.
                if (!d.valid()) {
                    continue;
                }

                if (d.tex.valid()) { // Newly added.
                    rnd->draw(d.tex)
                        .to(to_hal(d.staged))
                        .render();
                } else { // Present in the old atlas texture.
                    rnd->draw(texture)
                        .from(d.area)
                        .to(to_hal(d.staged))
                        .render();
                }

                d.area = to_hal(d.staged);
            }

            return canvas;
        }
    };
}
