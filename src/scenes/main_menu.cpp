#include <quest/scenes/main_menu.hpp>

#include <quest/constants.hpp>
#include <quest/helpers.hpp>

using namespace HQ::scene;

namespace lc { // Local constants.
    constexpr hal::color        colors[] { 0x000000, 0x0d0145, 0x5c0501, 0x01451a };
    constexpr hal::coord::point pt { 0.02, 0.1 };
}

main_menu::main_menu(hal::renderer& rnd, hal::ttf::context& ttf)
    : base {}
    , m_theme { rnd.color() }
    , m_currentTheme { std::size(lc::colors) - 1 } {

    const hal::font font { find_sized_font(ttf, "assets/Ubuntu Mono.ttf", rnd.size().y * 0.1) };

    constexpr std::string_view texts[] { "New game", "Continue", "Settings", "Exit" };
    static_assert(std::tuple_size_v<decltype(m_widgets)> == std::size(texts));

    auto               offset = rnd.size() * lc::pt;
    const hal::pixel_t sz { font.size_text(" ").y };
    hal::coord_t       accum { offset.y };

    for (std::size_t i { 0 }; i < std::size(texts); ++i) {
        m_widgets[i] = { rnd.make_texture(font.render(texts[i])(hal::font::render_type::blended)), { offset.x, accum } };
        accum += sz;
    }
}

void main_menu::process(const hal::event::handler& event) {
    switch (event.kind()) {
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
                if (static_cast<hal::coord::point>(event.mouse_button().pos()) | wgt.hitbox) {
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

type main_menu::update(delta_t elapsed, hal::renderer& rnd) {
    m_theme.update(elapsed);

    rnd.color(m_theme.value());

    for (const auto& wgt : m_widgets) {
        wgt.draw(rnd);
    }

    return type::none;
}

void main_menu::switch_theme() {
    using namespace hal::palette;

    m_currentTheme = ++m_currentTheme % std::size(lc::colors);

    m_theme.start(lc::colors[m_currentTheme], 0.5);
}