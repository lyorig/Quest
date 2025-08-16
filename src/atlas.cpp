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
}

void texture_atlas::queue(hal::static_texture tex, hal::pixel::rect& out) {
    m_data.emplace_back(std::move(tex), pt2rect(tex.size().get()), &out);
}

void texture_atlas::add(hal::static_texture tex, hal::pixel::rect& out, hal::ref<hal::renderer> rnd) {
    queue(std::move(tex), out);
    pack(rnd);
}

void texture_atlas::replace(hal::static_texture tex, hal::pixel::rect& out, hal::ref<hal::renderer> rnd) {
    free(out);
    add(std::move(tex), out, rnd);
}

void texture_atlas::free(hal::pixel::rect r) {
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

    static_assert(sizeof(hal::pixel::rect) == sizeof(rect_t));

    HAL_DEBUG_TIMER(tmr);

    std::vector<rect_t> rects(m_data.size());
    std::ranges::transform(m_data, rects.begin(), [](const data& d) { return d.taken; });

    // Find the best possible packing for these rects...
    const auto size = r2d::find_best_packing<spaces_t>(
        rects,
        r2d::make_finder_input(
            max_side,
            discard_step,
            [](const rect_t&) { return cr::CONTINUE_PACKING; },
            [](const rect_t&) { return cr::CONTINUE_PACKING; },
            r2d::flipping_option::DISABLED));

    this->texture = { rnd, { size.w, size.h } };

    hal::guard::target t { rnd, this->texture };

    // A tuple of (data, rect_t).
    for (const auto& tuple : std::views::zip(m_data, rects)) {
        auto& d = std::get<0>(tuple);
        d.taken = std::get<1>(tuple);

        const auto& taken {
            reinterpret_cast<const hal::pixel::rect&>(d.taken)
        };

        rnd->draw(d.tex)
            .to(taken)
            .render();

        // The original texture is no longer needed at this point.
        d.tex.reset();

        *d.out = taken;
    }

    HAL_PRINT("<Atlas> pack() finished in ", tmr);
}
