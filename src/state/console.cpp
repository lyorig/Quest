#include <quest/state/console.hpp>

using namespace HQ::state;

namespace consts {
    constexpr std::string_view font_path { "assets/Ubuntu Mono.ttf" };
    constexpr hal::u8          font_size { 32 };

    constexpr std::string_view pfx_text { "lyo@Engine ~ %" };
    constexpr hal::color       pfx_color { hal::palette::green };
    constexpr hal::pixel_t     pfx_padding { 20 };

    constexpr hal::color input_color { hal::palette::white };

    constexpr hal::color ph_color { 0x808080 };

    constexpr hal::coord_point offset { 10, 10 };

    constexpr hal::color bg_color { 0x000B3B, 192 };
}

console::console(hal::ttf::context& ttf)
    : m_font { ttf.load(hal::access(consts::font_path), consts::font_size) }
    , m_texBegin { static_cast<hal::pixel_t>(consts::offset.x + m_font.size_text(consts::pfx_text).x + consts::pfx_padding) }
    , m_repaint { false } {
}

void console::draw(hal::renderer& rnd) {

    hal::renderer::color_lock lock { rnd, consts::bg_color };
    rnd.fill_target();

    if (m_repaint) {
        m_repaint = false;
        repaint(rnd);
    }

    auto pos = consts::offset;

    rnd.draw(m_pfx).to(pos)();

    pos.x += m_texBegin;
    rnd.draw(m_tex).to(pos)();
}

void console::process(hal::keyboard::key k, hal::keyboard::mod_state m) {
    m_repaint = m_field.process(k, m) != text_field::op::nothing;
}

void console::process(char ch) {
    m_field.process(ch);
    m_repaint = true;
}

void console::show(hal::renderer& rnd) {
    m_repaint = true;

    m_pfx = rnd.make_texture(m_font.render(consts::pfx_text, consts::pfx_color).convert());
}

void console::hide() {
    m_pfx.reset();
    m_tex.reset();
}

bool console::active() {
    return m_field.has_focus();
}

bool console::toggle(hal::renderer& rnd) {
    if (m_field.toggle()) {
        show(rnd);
        return true;
    } else {
        hide();
        return false;
    }
}

void console::repaint(hal::renderer& rnd) {
    hal::surface text;

    if (m_field.text().empty()) {
        text = m_font.render(random_placeholder_text(), consts::ph_color);
    } else {
        text = m_font.render(m_field.text() + '|', consts::input_color);
    }

    m_tex = rnd.make_texture(text);
}

std::string_view console::random_placeholder_text() {
    constexpr std::string_view phrases[] {
        "[enter command here]",
        "[be not afraid]",
        "[food for thought?]",
        "[waiting for user input]",
        "rm -rf / --no-preserve-root",
        "[at your service]",
        "[not POSIX compliant]"
    };

    return phrases[std::rand() % std::size(phrases)];
}