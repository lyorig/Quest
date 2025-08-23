#include <quest/scene/main_menu.hpp>

#include <halcyon/utility/guard.hpp>

#include <quest/constants.hpp>
#include <quest/game.hpp>
#include <quest/helpers.hpp>
#include <ranges>

using namespace hq::scene;

namespace lc { // Local constants.
    constexpr hal::coord_t pt { 0.02 };
}

namespace {
    template <typename T, std::size_t N>
    const T& back(const T (&arr)[N]) {
        return arr[N - 1];
    }
}

main_menu::main_menu(game& g)
    : base { flag::all_enable } {
    const hal::font font { find_sized_font(g, "assets/Ubuntu Mono.ttf", static_cast<hal::pixel_t>(g.renderer.size()->y * 0.1)) };

    constexpr std::string_view texts[] { "New game", "Continue", "Settings", "Exit" };
    static_assert(hal::meta::array_size<decltype(m_widgets)> == std::size(texts));

    const hal::coord_t offset = g.renderer.size()->x * lc::pt;
    const hal::pixel_t sz { size_text(font, " ").y };
    hal::coord_t       accum { offset };

    for (const auto& tuple : std::views::zip(m_widgets, texts)) {
        std::get<0>(tuple) = { sprite { g, font.render_blended(std::get<1>(tuple), hal::colors::white), { offset, accum } } };
        accum += sz;
    }

    HAL_PRINT("<Main menu> Initialized.");
}

void main_menu::process(game& g) {
    for (const auto& e : g.polled())
        switch (e.kind()) {
            using enum hal::event::type;

        case mouse_pressed:
            if (e.mouse_button().button() == hal::mouse::button::left) {
                if (static_cast<hal::coord::point>(e.mouse_button().pos()) | back(m_widgets).s.hitbox) {
                    g.running = false;
                    break;
                }
            }
            break;

        case mouse_moved:
            for (auto& wgt : m_widgets) {
                const bool hit { static_cast<hal::coord::point>(e.mouse_motion().pos()) | wgt.s.hitbox };

                if (hit && wgt.d == widget::dir::down) {
                    wgt.c.start({ hal::colors::cyan, 128 }, 0.2);
                    wgt.d = widget::dir::up;
                } else if (!hit && wgt.d == widget::dir::up) {
                    wgt.c.start(hal::colors::white, 0.2);
                    wgt.d = widget::dir::down;
                }
            }
            break;

        default:
            break;
        }
}

void main_menu::update(game& g) {
    const delta_t d { g.delta() };

    for (auto& wgt : m_widgets) {
        wgt.c.update(d);
    }
}

void main_menu::draw(game& g) {
    for (const auto& wgt : m_widgets) {
        hal::guard::color_mod<hal::target_texture> guard { g.atlas.texture, wgt.c.value() };
        wgt.s.draw(g);
    }
}
