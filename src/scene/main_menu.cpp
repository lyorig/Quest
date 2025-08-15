#include <quest/scene/main_menu.hpp>

#include <halcyon/utility/guard.hpp>

#include <quest/constants.hpp>
#include <quest/game.hpp>
#include <quest/helpers.hpp>

using namespace hq::scene;

namespace lc { // Local constants.
    constexpr hal::color   colors[] { 0x000000, 0xd3ad00, 0x0d0145, 0x5c0501, 0x01451a };
    constexpr hal::coord_t pt { 0.02 };
}

main_menu::main_menu(game& g)
    : base { flag::all_enable }
    , m_theme { g.renderer.color().get() }
    , m_currentTheme { 0 } {
    const hal::font font { find_sized_font(g, "assets/Ubuntu Mono.ttf", static_cast<hal::pixel_t>(g.renderer.size()->y * 0.1)) };

    constexpr hal::c_string texts[] { "New game", "Continue", "Settings", "Exit" };
    static_assert(std::tuple_size_v<decltype(m_widgets)> == std::size(texts));

    const hal::coord_t offset = g.renderer.size()->x * lc::pt;
    const hal::pixel_t sz { size_text(font, " ").y };
    hal::coord_t       accum { offset };

    for (std::size_t i { 0 }; i < std::size(texts); ++i) {
        m_widgets[i] = widget { { g, font.render_blended(texts[i], hal::colors::white), { offset, accum } }, hal::colors::white };
        accum += sz;
    }

    g.renderer.color(lc::colors[m_currentTheme]);

    HAL_PRINT("<Main menu> Initialized.");
}

void main_menu::process(game& g) {
    for (const auto& e : g.polled())
        switch (e.kind()) {
            using enum hal::event::type;

        case key_pressed:
            switch (e.keyboard().key()) {
                using enum hal::keyboard::key;

            case C:
                switch_theme();
                break;

            default:
                break;
            }
            break;

        case mouse_pressed:
            if (e.mouse_button().button() == hal::mouse::button::left) {
                if (static_cast<hal::coord::point>(e.mouse_button().pos()) | m_widgets.back().s.hitbox) {
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

    m_theme.update(d);

    for (auto& wgt : m_widgets) {
        if (wgt.c.update(d)) {
            wgt.mod = wgt.c.value();
        }
    }
}

void main_menu::draw(game& g) {
    g.renderer.fill(m_theme.value());

    for (const auto& wgt : m_widgets) {
        hal::guard::color_mod guard { g.atlas.texture, wgt.mod };
        wgt.s.draw(g);
    }
}

void main_menu::switch_theme() {
    using namespace hal::colors;

    ++m_currentTheme;
    m_currentTheme %= std::size(lc::colors);

    m_theme.start(lc::colors[m_currentTheme], 0.5);
}
