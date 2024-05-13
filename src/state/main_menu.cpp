#include <quest/state/main_menu.hpp>

using namespace HQ::state;

namespace consts {
    constexpr hal::coord_point offset { 50, 50 };

    constexpr hal::font::render_type text_render_type { hal::font::render_type::blended };
}

main_menu::main_menu(hal::renderer& rnd, hal::ttf::context& ttf)
    : m_theme { rnd.color() }
    , m_currentTheme { 0 } {

    const hal::font font { ttf.load(hal::access("assets/Ubuntu Mono.ttf"), 128) };

    const hal::surface surf { font.render("Main Menu")(consts::text_render_type) };

    const auto pos = hal::coord_rect(hal::tag::as_size, rnd.size()).anchor(hal::anchor::bottom_right, surf.size()) - consts::offset;

    m_widgets.push_back({ rnd.make_texture(surf), pos, surf.size() });
}

void main_menu::process(const hal::event::handler& event) {
    switch (event.event_type()) {
        using enum hal::event::type;

    case key_pressed:
        switch (event.keyboard().key()) {
            using enum hal::keyboard::key;

        case C:
            switch_theme();
            break;

        default:
            break;
        }
        break;

    case mouse_pressed:
        if (event.mouse_button().button() == hal::mouse::button::left) {
            for (const auto& wgt : m_widgets) {
                if (event.mouse_button().pos() | wgt.hitbox) {
                    switch_theme();
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

base* main_menu::update(delta_t elapsed) {
    m_theme.update(elapsed);

    return nullptr;
}

void main_menu::draw(hal::renderer& rnd) const {
    rnd.color(m_theme.value());

    for (const auto& wgt : m_widgets) {
        wgt.draw(rnd);
    }
}

void main_menu::switch_theme() {
    using namespace hal::palette;

    // Technically, this starts on the 2nd color.
    constexpr hal::color colors[] { blue, red, black, orange };
    m_currentTheme = ++m_currentTheme % std::size(colors);

    m_theme.start(colors[m_currentTheme], 0.5);
}