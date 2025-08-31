#include <quest/atlas.hpp>

#include <quest/game.hpp>

#include <halcyon/surface.hpp>
#include <halcyon/system.hpp>
#include <halcyon/utility/guard.hpp>
#include <halcyon/utility/timer.hpp>
#include <halcyon/video/renderer.hpp>

#include <ranges>

using namespace hq;

namespace {
    constexpr std::size_t  MAX_SIDE { 1024 };
    constexpr std::int32_t DISCARD_STEP { -4 };

    constexpr hal::pixel::rect to_hal(texture_atlas::rect_t r) {
        return std::bit_cast<hal::pixel::rect>(r);
    }

    constexpr texture_atlas::rect_t to_r2d(hal::pixel::point size) {
        return { 0, 0, size.x, size.y };
    }

    std::future<hal::static_texture> create_async(game& g, hal::surface s) {
        // Linux & Windows both don't play well with textures being created in a separate thread.
        if constexpr (hal::platform::is_macos()) {
            // Capture by value, or `rnd` will get destructed and you'll
            // be left with a `nullptr` texture (or some sort of UB)!
            return g.pool.run([rnd = hal::ref { g.renderer }, s = std::move(s)] {
                return hal::static_texture { rnd, s };
            });
        } else {
            hal::static_texture t { g.renderer, s };

            return g.pool.run([t = std::move(t)] mutable {
                return std::move(t);
            });
        }
    }
}

texture_atlas::texture_atlas()
    : m_repack { false } { }

texture_atlas::id texture_atlas::add(game& g, hal::surface surf) {
    m_repack = true;

    const rect_t        rect { to_r2d(surf.size()) };
    hal::static_texture tex { g.renderer, std::move(surf) };

    std::underlying_type_t<id> i { 0 };

    // Can we reuse a slot in the vector?
    for (; i < m_data.size(); ++i) {
        data& d { m_data[i] };

        if (!d.valid()) {
            d.tex    = create_async(g, std::move(surf));
            d.staged = rect;

            return static_cast<id>(i);
        }
    }

    m_data.emplace_back(
        hal::pixel::rect {},
        rect,
        create_async(g, std::move(surf)));

    return static_cast<id>(i);
}

void texture_atlas::replace(id id, game& g, hal::surface surf) {
    data& d { m_data[std::to_underlying(id)] };

    if (surf.size() == d.area) {
        return replace_exact(id, g.renderer, std::move(surf));
    }

    m_repack = true;

    d.staged = to_r2d(surf.size());
    d.tex    = create_async(g, std::move(surf));
}

void texture_atlas::replace_exact(id id, hal::ref<hal::renderer> rnd, hal::surface surf) {
    hal::guard::target  _ { rnd, texture };
    hal::static_texture tex { rnd, std::move(surf) };

    rnd->draw(tex).to(m_data[std::to_underlying(id)].area).render();
}

void texture_atlas::free(id id) {
    m_data[std::to_underlying(id)].invalidate();
}

void texture_atlas::pack(hal::ref<hal::renderer> rnd) {
    using cr = r2d::callback_result;

    // Check whether there's actually anything to do.
    if (!m_repack) {
        return;
    }

    m_repack = false;

    // Find the best possible packing for these rects...
    const r2d::rect_wh size { r2d::find_best_packing<spaces_t>(
        m_data,
        r2d::make_finder_input(
            MAX_SIDE,
            DISCARD_STEP,
            [](const rect_t&) { return cr::CONTINUE_PACKING; },
            [](const rect_t&) { return cr::ABORT_PACKING; },
            r2d::flipping_option::DISABLED)) };

    // ...then create the texture itself.
    texture = create(rnd, std::bit_cast<hal::pixel::point>(size));
}

void texture_atlas::gc() {
    const auto subrange {
        std::ranges::find_last_if(m_data, [](const data& d) { return !d.valid(); })
    };

    m_data.erase(subrange.begin(), subrange.end());
}

hal::target_texture texture_atlas::create(hal::ref<hal::renderer> rnd, hal::pixel::point sz) {
    hal::target_texture canvas { rnd, sz };

    // FIXME Metal apparently waits for VSync upon setting the render target.
    // https://discourse.libsdl.org/t/sdl-setrendertargets-metal-implementation-apparently-waits-for-vsync/62009.
    hal::guard::target _ { rnd, canvas };

    // Clear the texture (added after noticing weird graphical glitches on Windows).
    rnd->clear();

    for (data& d : m_data) {
        // Skip unused entries.
        if (!d.valid()) {
            continue;
        }

        if (d.tex.valid()) { // Newly added.
            rnd->draw(d.tex.get())
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

texture_atlas_copyer texture_atlas::draw(id id, hal::ref<hal::renderer> rnd) {
    HAL_ASSERT(m_data[std::to_underlying(id)].valid(), "Drawing invalid texture");

    return { hal::pass_key<texture_atlas> {}, rnd->draw(texture).from(m_data[std::to_underlying(id)].area) };
}

hal::pixel::rect texture_atlas::area(id id) const {
    return m_data[std::to_underlying(id)].area;
}

void texture_atlas::debug_draw(
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

texture_atlas::rect_t& texture_atlas::data::get_rect() {
    return staged;
}

const texture_atlas::rect_t& texture_atlas::data::get_rect() const {
    return staged;
}

void texture_atlas::data::invalidate() {
    staged.x = -1;
}

bool texture_atlas::data::valid() const {
    return staged.x != -1;
}

using tac = texture_atlas_copyer;

tac::texture_atlas_copyer(hal::pass_key<texture_atlas>, hal::copyer c)
    : hal::copyer { std::move(c) } {
}

tac& tac::from(hal::pixel::rect src) {
    src.pos += this->m_posSrc.pos;
    return static_cast<texture_atlas_copyer&>(copyer::from(src));
}
