#include <quest/atlas.hpp>

#include <halcyon/surface.hpp>
#include <halcyon/utility/guard.hpp>
#include <halcyon/utility/timer.hpp>
#include <halcyon/video/renderer.hpp>

using namespace hq;

namespace {
    constexpr std::size_t  MAX_SIDE { 1024 };
    constexpr std::int32_t DISCARD_STEP { -4 };
    constexpr hal::pixel_t INVALID_POS { -1 };
}

texture_atlas::texture_atlas()
    : m_repack { false } { }

texture_atlas::id_t texture_atlas::add(hal::ref<hal::renderer> rnd, hal::surface surf) {
    m_repack = true;

    const hal::pixel::rect rect { hal::tag::as_size, surf.size() };
    hal::static_texture    tex { rnd, std::move(surf) };

    id_t i { 0 };

    // Can we reuse a slot in the vector?
    for (; i < m_data.size(); ++i) {
        data& d { m_data[i] };

        if (d.staged.pos.x == INVALID_POS) {
            d.tex    = std::move(tex);
            d.staged = rect;

            return i;
        }
    }

    m_data.emplace_back(
        hal::pixel::rect {},
        rect,
        std::move(tex));

    return i;
}

void texture_atlas::replace(id_t id, hal::ref<hal::renderer> rnd, hal::surface surf) {
    data& d { m_data[id] };

    if (surf.size() == d.area) {
        return replace_exact(id, rnd, std::move(surf));
    }

    m_repack = true;

    d.staged = { hal::tag::as_size, surf.size() };
    d.tex    = { rnd, std::move(surf) };
}

void texture_atlas::replace_exact(id_t id, hal::ref<hal::renderer> rnd, hal::surface surf) {
    hal::guard::target  _ { rnd, texture };
    hal::static_texture tex { rnd, std::move(surf) };

    rnd->draw(tex).to(m_data[id].area).render();
}

void texture_atlas::free(id_t id) {
    m_data[id].staged.pos.x = INVALID_POS;
}

void texture_atlas::pack(hal::ref<hal::renderer> rnd) {
    using cr = r2d::callback_result;

    // Check whether there's actually anything to do.
    if (!m_repack) {
        return;
    }

    m_repack = false;

    // Find the best possible packing for these rects...
    const auto size = r2d::find_best_packing<spaces_t>(
        m_data,
        r2d::make_finder_input(
            MAX_SIDE,
            DISCARD_STEP,
            [](const rect_t&) { return cr::CONTINUE_PACKING; },
            [](const rect_t&) { return cr::ABORT_PACKING; },
            r2d::flipping_option::DISABLED));

    // ...then create the texture itself.
    texture = create(rnd, std::bit_cast<hal::pixel::point>(size));
}

hal::target_texture texture_atlas::create(hal::ref<hal::renderer> rnd, hal::pixel::point sz) {
    hal::target_texture canvas { rnd, sz };
    hal::guard::target  _ { rnd, canvas };

    // Clear the texture (added after noticing weird graphical glitches on Windows).
    // FIXME: Atlas debug drawing fails
    rnd->clear();

    for (data& d : m_data) {
        // Skip unused entries.
        if (d.staged.pos.x == INVALID_POS) {
            continue;
        }

        if (d.tex.valid()) { // Newly added.
            rnd->draw(d.tex)
                .to(d.staged)
                .render();

            d.tex.reset();
        } else { // Present in the old atlas texture.
            rnd->draw(texture)
                .from(d.area)
                .to(d.staged)
                .render();
        }

        d.area = d.staged;
    }

    return canvas;
}

texture_atlas_copyer texture_atlas::draw(id_t id, hal::ref<hal::renderer> rnd) {
    HAL_ASSERT(m_data[id].staged.pos.x != INVALID_POS, "Drawing invalid texture");

    return { hal::pass_key<texture_atlas> {}, rnd->draw(texture).from(m_data[id].area) };
}

hal::pixel::rect texture_atlas::area(id_t id) const {
    return m_data[id].area;
}

void texture_atlas::debug_draw(
    hal::ref<hal::renderer> rnd,
    hal::coord::point       dst,
    hal::color              outline_atlas,
    hal::color              outline_block) const {
    rnd->draw(texture).to(dst).render();

    for (const data& d : m_data) {
        if (d.staged.pos.x == INVALID_POS) {
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

constexpr texture_atlas::rect_t& texture_atlas::data::get_rect() {
    return reinterpret_cast<rect_t&>(staged);
}

using tac = texture_atlas_copyer;

tac::texture_atlas_copyer(hal::pass_key<texture_atlas>, hal::copyer c)
    : hal::copyer { std::move(c) } {
}

tac& tac::from(hal::pixel::rect src) {
    src.pos += this->m_posSrc.pos;
    return static_cast<texture_atlas_copyer&>(copyer::from(src));
}
