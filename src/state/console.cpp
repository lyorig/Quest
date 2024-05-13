#include <quest/state/console.hpp>

#include <halcyon/utility/locks.hpp>

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

    constexpr hal::font::render_type text_render_type { hal::font::render_type::blended };
}

console::console(hal::ttf::context& ttf)
    : m_col { { 0, 0 } }
    , m_font { ttf.load(hal::access(consts::font_path), consts::font_size) }
    , m_texBegin { static_cast<hal::pixel_t>(consts::offset.x + m_font.size_text(consts::pfx_text).x + consts::pfx_padding) }
    , m_repaint { false } {
}

void console::update(delta_t elapsed, hal::renderer& rnd) {
    if (active()) {
        hal::lock::color lock { rnd, m_col.update(elapsed) ? m_col.value() : consts::bg_color };
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

    m_col.jump({ 0, 0 });
    m_col.start(consts::bg_color, 0.1);

    m_pfx = rnd.make_texture(m_font.render(consts::pfx_text).fg(consts::pfx_color)(consts::text_render_type));
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
        text = m_font.render(random_placeholder_text())
                   .fg(consts::ph_color)(consts::text_render_type);
    } else {
        text = m_font.render(m_field.text() + '|')
                   .fg(consts::input_color)(consts::text_render_type);
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
        "[not POSIX compliant]",
        "[made with Halcyon]"
    };

    return phrases[std::rand() % std::size(phrases)];
}