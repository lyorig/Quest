#include <quest/atlas.hpp>

#include <halcyon/surface.hpp>
#include <halcyon/utility/guard.hpp>
#include <halcyon/utility/timer.hpp>
#include <halcyon/video/renderer.hpp>

using namespace hq;

namespace {
    constexpr std::size_t  MAX_SIDE { 1024 };
    constexpr std::int32_t DISCARD_STEP { -4 };
}

texture_atlas::texture_atlas()
    : m_repack { false } { }

texture_atlas::id_t texture_atlas::add(hal::surface surf) {
    m_repack = true;

    const hal::pixel::rect rect { hal::tag::as_size, surf.size() };

    id_t i { 0 };

    // Can we reuse a slot in the vector?
    for (; i < m_data.size(); ++i) {
        data& d { m_data[i] };

        if (!d.valid()) {
            d.tex    = std::move(surf);
            d.staged = rect;

            return i;
        }
    }

    m_data.emplace_back(
        hal::pixel::rect {},
        rect,
        std::move(surf));

    return i;
}

void texture_atlas::replace(id_t id, hal::surface surf) {
    data& d { m_data[id] };

    if (surf.size() == d.area) {
        return replace_exact(id, std::move(surf));
    }

    m_repack = true;

    d.staged = { hal::tag::as_size, surf.size() };
    d.tex    = std::move(surf);
}

void texture_atlas::replace_exact(id_t id, hal::surface surf) {
    // FIXME This doesn't sync with the texture!
    surf.blit(surface).to(m_data[id].area).blit();
}

void texture_atlas::free(id_t id) {
    m_data[id].invalidate();
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

    hal::timer t;

    // ...then create the texture itself.
    this->surface = create(std::bit_cast<hal::pixel::point>(size));
    this->texture = { rnd, create(std::bit_cast<hal::pixel::point>(size)) };

    std::println("<Atlas> Surface/texture creation took {:9f}s", t.get());
}

hal::surface texture_atlas::create(hal::pixel::point sz) {
    hal::surface canvas { sz, hal::pixel::format::rgba32 };

    for (data& d : m_data) {
        // Skip unused entries.
        if (!d.valid()) {
            continue;
        }

        if (d.tex.valid()) { // Newly added.
            d.tex.blit(canvas)
                .to(d.staged)
                .blit();

            d.tex.reset();
        } else { // Present in the old atlas texture.
            this->surface.blit(canvas)
                .from(d.area)
                .to(d.staged)
                .blit();
        }

        d.area = d.staged;
    }

    return canvas;
}

texture_atlas_copyer texture_atlas::draw(id_t id, hal::ref<hal::renderer> rnd) {
    HAL_ASSERT(m_data[id].valid(), "Drawing invalid texture");

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
        if (!d.valid()) {
            continue;
        }

        hal::pixel::rect rect { d.area };
        rect.pos += dst;
        rect.pos += { 1, 1 };
        rect.size -= { 2, 2 };

        rnd->draw(rect, outline_block);
    }

    rnd->draw({ dst, surface.size() }, outline_atlas);
}

texture_atlas::rect_t& texture_atlas::data::get_rect() {
    return reinterpret_cast<rect_t&>(staged);
}

const texture_atlas::rect_t& texture_atlas::data::get_rect() const {
    return reinterpret_cast<const rect_t&>(staged);
}

void texture_atlas::data::invalidate() {
    staged.pos.x = -1;
}

bool texture_atlas::data::valid() const {
    return staged.pos.x != -1;
}

using tac = texture_atlas_copyer;

tac::texture_atlas_copyer(hal::pass_key<texture_atlas>, hal::copyer c)
    : hal::copyer { std::move(c) } {
}

tac& tac::from(hal::pixel::rect src) {
    src.pos += this->m_posSrc.pos;
    return static_cast<texture_atlas_copyer&>(copyer::from(src));
}
