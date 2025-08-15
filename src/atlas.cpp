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
    constexpr bool memsame(const A& lhs, const B& rhs) {
        return std::memcmp(&lhs, &rhs, sizeof(A)) == 0;
    }
}

void texture_atlas::queue(hal::static_texture tex, hal::pixel::rect& out) {
    m_queued.emplace_back(std::move(tex), &out);
}

void texture_atlas::free(hal::pixel::rect r) {
    for (auto& foo : m_taken) {
        if (memsame(foo, r)) {
            std::swap(foo, m_taken.back());
            m_taken.pop_back();
            return;
        }
    }
}

void texture_atlas::pack(hal::ref<hal::renderer> rnd) {
    using cr = r2d::callback_result;

    static_assert(sizeof(hal::pixel::rect) == sizeof(rect_t));

    m_taken.resize(this->m_queued.size());
    std::ranges::transform(this->m_queued.begin(), this->m_queued.end(), m_taken.begin(), [](const data& t) {
        const hal::pixel::point size { t.tex.size().get() };
        return rect_t { 0, 0, size.x, size.y };
    });

    const auto size = r2d::find_best_packing<spaces_t>(
        m_taken,
        r2d::make_finder_input(
            max_side,
            discard_step,
            [](const rect_t&) { return cr::CONTINUE_PACKING; },
            [](const rect_t&) { return cr::CONTINUE_PACKING; },
            r2d::flipping_option::DISABLED));

    {
        hal::guard::target t { rnd, this->texture };

        for (const auto& zip : std::views::zip(m_taken, m_queued)) {
            const auto& foo {
                reinterpret_cast<const hal::pixel::rect&>(std::get<0>(zip))
            };

            const auto& bar {
                std::get<1>(zip)
            };

            rnd->draw(bar.tex)
                .to(foo)
                .render();

            *bar.out = foo;
        }
    }

    this->texture = { rnd, { size.w, size.h } };
}
