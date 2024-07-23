#include <quest/scene/main_menu.hpp>

#include <halcyon/utility/locks.hpp>

#include <quest/constants.hpp>
#include <quest/game.hpp>
#include <quest/helpers.hpp>

using namespace hq::scene;

namespace lc { // Local constants.
    using namespace hal::literals;

    constexpr hal::color        colors[] { 0xd3ad00, 0x0d0145, 0x5c0501, 0x01451a };
    constexpr hal::coord::point pt { 0.02_crd, 0.1_crd };

    constexpr hal::coord_t invalid_outline { std::numeric_limits<hal::coord_t>::infinity() };
}

main_menu::main_menu(game& g)
    : base { flags::all_enable }
    , m_theme { g.renderer.color() }
    , m_outline { { .x = lc::invalid_outline } }
    , m_currentTheme { static_cast<hal::u8>(std::size(lc::colors) - 1) } {
    const hal::font font { find_sized_font(g.ttf, "assets/Ubuntu Mono.ttf", static_cast<hal::pixel_t>(g.renderer.size().y * 0.1)) };

    constexpr std::string_view texts[] { "New game", "Continue", "Settings", "Exit" };
    static_assert(std::tuple_size_v<decltype(m_widgets)> == std::size(texts));

    auto               offset = g.renderer.size() * lc::pt;
    const hal::pixel_t sz { font.size_text(" ").y };
    hal::coord_t       accum { offset.y };

    for (std::size_t i { 0 }; i < std::size(texts); ++i) {
        m_widgets[i] = { g.renderer.make_static_texture(font.render(texts[i])(hal::font::render_type::blended)), { offset.x, accum } };
        accum += sz;
    }
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

        case mouse_moved:
            for (const auto& wgt : m_widgets) {
                if (static_cast<hal::coord::point>(e.mouse_motion().pos()) | wgt.hitbox) {
                    m_outline = wgt.hitbox;
                    goto Breakout;
                }
            }

            m_outline.pos.x = lc::invalid_outline;

        Breakout:
            break;

        case mouse_pressed:
            if (e.mouse_button().button() == hal::mouse::button::left) {
                if (static_cast<hal::coord::point>(e.mouse_button().pos()) | m_widgets.back().hitbox) {
                    g.running = false;
                    break;
                }
            }
            break;

        default:
            break;
        }
}

void main_menu::update(game& g) {
    m_theme.update(g.delta());
}

void main_menu::draw(game& g) {
    hal::renderer& rnd { g.renderer };

    rnd.color(m_theme.value());

    for (const auto& wgt : m_widgets) {
        wgt.draw(rnd);

        if (m_outline.pos.x != lc::invalid_outline) {
            hal::lock::color<hal::renderer> cl { rnd, { 0x00FFFF, 32 } };

            rnd.fill(m_outline);
        }
    }
}

void main_menu::activate(game&) {
}

void main_menu::deactivate() {
}

std::string_view main_menu::name() const {
    return "Main menu";
}

void main_menu::switch_theme() {
    using namespace hal::palette;

    m_currentTheme = ++m_currentTheme % std::size(lc::colors);

    m_theme.start(lc::colors[m_currentTheme], 0.5);
}
