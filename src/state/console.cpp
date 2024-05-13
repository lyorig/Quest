#include <quest/state/console.hpp>

using namespace HQ::state;

namespace consts {
    constexpr std::string_view font_path { "assets/Ubuntu Mono.ttf" };
    constexpr hal::u8          font_size { 64 };

    constexpr std::string_view pfx_text { "> " };
    constexpr hal::color       input_color { hal::palette::white };

    constexpr std::string_view ph_text { "[enter command here...]" };
    constexpr hal::color       ph_color { 0x808080 };

    constexpr hal::coord_point offset { 10, 10 };
    constexpr hal::pixel_t     line_height { 32 };

    constexpr hal::color bg_color { 0x000B3B, 192 };
}

console::console(hal::ttf::context& ttf)
    : m_font { ttf.load(hal::access(consts::font_path), consts::font_size) }
    , m_glyphSize { m_font.size_text("A") }
    , m_repaint { text_field::op::nothing } {
}

void console::draw(hal::renderer& rnd) {

    hal::renderer::color_lock lock { rnd, consts::bg_color };
    rnd.fill_target();

    switch (m_repaint) {
        using enum text_field::op;

    case nothing:
        break;

    case add:
        m_repaint = nothing;
        append_char(m_field.text().back(), rnd);
        break;

    case remove:
        m_repaint = nothing;
        remove_chars(rnd);
        break;
    }

    rnd.draw(m_tex).to(consts::offset)();
}

void console::process(hal::keyboard::key k, hal::keyboard::mod_state m) {
    m_repaint = m_field.process(k, m);
}

void console::process(char ch) {
    m_field.process(ch);
    m_repaint = text_field::op::add;
}

void console::show() {
    m_repaint = text_field::op::remove;

    HAL_PRINT("Showing console");
}

void console::hide() {
    m_tex.reset();

    HAL_PRINT("Hiding console");
}

bool console::active() {
    return m_field.has_focus();
}

bool console::toggle() {
    if (m_field.toggle()) {
        show();
        return true;
    } else {
        hide();
        return false;
    }
}

void console::append_char(char ch, hal::renderer& rnd) {
    HAL_PRINT("Adding");
    hal::pixel_point total_size = m_tex.size();
    total_size.x += m_glyphSize.x;

    hal::target_texture tmp = rnd.make_target_texture(total_size);

    hal::renderer::target_lock tl { rnd, tmp };

    rnd.draw(m_tex)();
    rnd.draw(rnd.make_texture(m_font.render_glyph(ch))).to({ m_tex.size().x, 0 })();

    m_tex = std::move(tmp);
}

void console::remove_chars(hal::renderer& rnd) {
    HAL_PRINT("Removing");
    std::string total { consts::pfx_text };

    if (m_field.text().empty()) {
        total += consts::ph_text;
    } else {
        total += m_field.text();
    }

    hal::pixel_point size {
        static_cast<hal::pixel_t>(m_glyphSize.x * total.size()),
        m_glyphSize.y
    };

    hal::target_texture tmp  = rnd.make_target_texture(size);
    hal::surface        curr = m_font.render(consts::pfx_text, consts::input_color);

    size.x = 0;
    size.y = 0;

    hal::renderer::target_lock lock { rnd, tmp };

    rnd.draw(rnd.make_texture(curr))();
    size.x += curr.size().x;

    if (m_field.text().empty()) {
        curr = m_font.render(consts::ph_text, consts::ph_color);
    } else {
        curr = m_font.render(m_field.text(), consts::input_color);
    }

    rnd.draw(rnd.make_texture(curr)).to(size)();

    m_tex = std::move(tmp);
}