#include <quest/state/console.hpp>

using namespace HQ::state;

namespace consts {
    constexpr std::string_view font_path { "assets/Ubuntu Mono.ttf" };
    constexpr hal::u8          font_size { 32 };

    constexpr std::string_view pfx_text { ">" };
    constexpr hal::color       pfx_color { hal::palette::green };
    constexpr hal::pixel_t     pfx_padding { 20 };

    constexpr hal::color input_color { hal::palette::white };

    constexpr std::string_view ph_text { "[enter command here...]" };
    constexpr hal::color       ph_color { 0x808080 };

    constexpr hal::coord_point offset { 10, 10 };

    constexpr hal::color bg_color { 0x000B3B, 192 };
}

console::console(hal::ttf::context& ttf)
    : m_font { ttf.load(hal::access(consts::font_path), consts::font_size) }
    , m_repaint { false } {
}

void console::draw(hal::renderer& rnd) {

    hal::renderer::color_lock lock { rnd, consts::bg_color };
    rnd.fill_target();

    if (m_repaint) {
        m_repaint = false;
        repaint(rnd);
    }

    rnd.draw(m_tex).to(consts::offset)();
}

void console::process(hal::keyboard::key k, hal::keyboard::mod_state m) {
    m_repaint = m_field.process(k, m) != text_field::op::nothing;
}

void console::process(char ch) {
    m_field.process(ch);
    m_repaint = true;
}

void console::show() {
    m_repaint = true;

    m_pfx = m_font.render(consts::pfx_text, consts::pfx_color);

    HAL_PRINT("Showing console");
}

void console::hide() {
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

void console::repaint(hal::renderer& rnd) {
    hal::surface text;

    if (m_field.text().empty()) {
        text = m_font.render(consts::ph_text, consts::ph_color);
    } else {
        text = m_font.render(m_field.text() + '|', consts::input_color);
    }

    hal::pixel_point size {
        static_cast<hal::pixel_t>(m_pfx.size().x + consts::pfx_padding + text.size().x),
        text.size().y
    };

    HAL_PRINT("Canvas size is ", size);

    hal::surface canvas { size };

    size.x -= text.size().x;
    size.y = 0;

    HAL_PRINT("Blitting text to ", size);

    m_pfx.blit(canvas)();
    text.blit(canvas).to(size)();

    m_tex = rnd.make_texture(canvas);
}