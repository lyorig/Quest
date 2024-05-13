#include <quest/state/console.hpp>

using namespace HQ::state;

namespace consts {
    constexpr std::string_view prefix { "> " };

    constexpr hal::coord_point offset { 10, 10 };
    constexpr hal::color       background { 0x000B3B, 192 };
}

console::console(hal::font&& fnt)
    : m_font { std::move(fnt) }
    , m_tex { consts::offset }
    , m_repaint { true } {
}

void console::draw(hal::renderer& rnd) {

    hal::renderer::color_lock lock { rnd, consts::background };
    rnd.fill_target();

    if (m_repaint) {
        m_repaint = false;

        std::string actual { consts::prefix };
        (actual += m_field.text()) += '|';
        m_tex.reset(rnd.make_texture(m_font.render(actual)), hal::scale::height(64));
    }

    m_tex.draw(rnd);
}

void console::process(hal::keyboard::key k, hal::keyboard::mod_state m) {
    m_repaint = m_field.process(k, m);
}

void console::process(char ch) {
    m_field.process(ch);
    m_repaint = true;
}

void console::show() {

    m_repaint = true;

    HAL_PRINT("Showing console");
}

void console::hide() {
    m_tex.texture.reset();

    HAL_PRINT("Hiding console");
}

bool console::active() {
    return m_field.has_focus();
}

bool console::toggle() {
    return m_field.toggle();
}