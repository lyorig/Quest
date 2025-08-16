#include "halcyon/surface.hpp"
#include "halcyon/utility/timer.hpp"
#include <quest/atlas.hpp>

#include <halcyon/utility/guard.hpp>
#include <halcyon/video/renderer.hpp>

#include <algorithm>
#include <ranges>

using namespace hq;

namespace {
    constexpr std::size_t  max_side { 1024 };
    constexpr std::int32_t discard_step { -4 };

    template <typename A, typename B>
        requires(sizeof(A) == sizeof(B))
    bool memsame(const A& lhs, const B& rhs) {
        return std::memcmp(&lhs, &rhs, sizeof(A)) == 0;
    }

    constexpr texture_atlas::rect_t pt2rect(hal::pixel::point pt) {
        return { 0, 0, pt.x, pt.y };
    }

    constexpr hal::pixel::rect rect2hal(texture_atlas::rect_t r) {
        return std::bit_cast<hal::pixel::rect>(r);
    }

    constexpr texture_atlas::rect_t hal2rect(hal::pixel::rect r) {
        return std::bit_cast<texture_atlas::rect_t>(r);
    }
}

void texture_atlas::queue(hal::static_texture tex, hal::pixel::rect& out) {
    m_data.emplace_back(pt2rect(tex.size().get()), std::move(tex), &out);
}

void texture_atlas::add(hal::surface surf, hal::pixel::rect& out, hal::ref<hal::renderer> rnd) {
    queue({ rnd, surf }, out);
    pack(rnd);
}

void texture_atlas::replace(hal::surface surf, hal::pixel::rect& out, hal::ref<hal::renderer> rnd) {
    free(out);
    add(std::move(surf), out, rnd);
}

void texture_atlas::free(hal::pixel::rect r) {
    if (r == hal::pixel::rect {}) {
        return;
    }

    for (auto iter = m_data.begin(); iter != m_data.end(); ++iter) {
        if (memsame(iter->taken, r)) {
            std::swap(*iter, m_data.back());
            m_data.pop_back();

            return;
        }
    }

    HAL_WARN("<Atlas> Freeing unknown rectangle ", r);
}

void texture_atlas::pack(hal::ref<hal::renderer> rnd) {
    using cr = r2d::callback_result;

    hal::timer t;

    std::vector<rect_t> rects(m_data.size());
    std::ranges::transform(m_data, rects.begin(), [](const data& d) { return d.taken; });

    // Find the best possible packing for these rects...
    const auto size = r2d::find_best_packing<spaces_t>(
        rects,
        r2d::make_finder_input(
            max_side,
            discard_step,
            [](const rect_t&) { return cr::CONTINUE_PACKING; },
            [](const rect_t&) { return cr::ABORT_PACKING; },
            r2d::flipping_option::DISABLED));

    // ...then create the texture itself.
    this->texture = create(rnd, { size.w, size.h }, rects);

    std::println("Took {}s", t());
}

hal::target_texture texture_atlas::create(hal::ref<hal::renderer> rnd, hal::pixel::point sz, std::span<const rect_t> rects) {
    hal::target_texture ret { rnd, sz };
    hal::guard::target  t { rnd, ret };

    // A tuple of (data, rect_t).
    for (const auto& tuple : std::views::zip(m_data, rects)) {
        auto& d       = std::get<0>(tuple);
        auto  new_pos = rect2hal(std::get<1>(tuple));

        if (d.tex.valid()) { // Newly added.
            rnd->draw(d.tex)
                .to(new_pos)
                .render();

            d.tex.reset();
        } else { // Present in the old atlas texture.
            auto old_pos = rect2hal(d.taken);

            rnd->draw(this->texture)
                .from(old_pos)
                .to(new_pos)
                .render();
        }

        d.taken = hal2rect(new_pos);
        *d.out  = new_pos;
    }

    return ret;
}

texture_atlas_copyer texture_atlas::draw(hal::ref<hal::renderer> rnd, hal::pixel::rect src) {
    return { hal::pass_key<texture_atlas> {}, rnd->draw(texture).from(src) };
}

void texture_atlas::debug_draw(
    hal::ref<hal::renderer> rnd,
    hal::coord::point       dst,
    hal::color              outline_atlas,
    hal::color              outline_block) const {
    for (const data& d : m_data) {
        auto rect = rect2hal(d.taken);
        rect.pos += dst;
        rnd->draw(rect, outline_block);
    }

    rnd->draw(texture).to(dst).outline(outline_atlas).render();
}

using tac = texture_atlas_copyer;

tac::texture_atlas_copyer(hal::pass_key<texture_atlas>, hal::copyer c)
    : hal::copyer { std::move(c) } {
}
